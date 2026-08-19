#!/usr/bin/env python3
"""Generate reproducible reachable origin/destination pairs from a routing HDF5 map."""

from __future__ import annotations

import argparse
import csv
from pathlib import Path
import random
import sys

import networkx as nx

from routing_hdf import distance_band, haversine_km, read_hdf_graph


# Editable no-argument configuration.
MAP_HDF5 = Path("data/prague.hdf5")
OUTPUT_CSV = Path("data/prague-od.csv")
PAIR_COUNT = 100
RANDOM_SEED = 42
BALANCE_DISTANCE_BANDS = True
DISTANCE_BANDS_KM = (3.0, 15.0, 50.0, 120.0)
OVERWRITE = False

FIELDNAMES = [
    "pair_id",
    "origin",
    "destination",
    "origin_osm_id",
    "destination_osm_id",
    "origin_lat",
    "origin_lon",
    "destination_lat",
    "destination_lon",
    "flight_distance_km",
    "distance_band",
]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--map", type=Path, dest="map_path")
    parser.add_argument("--output", type=Path)
    parser.add_argument("--count", type=int)
    parser.add_argument("--seed", type=int)
    parser.add_argument("--balanced", action=argparse.BooleanOptionalAction, default=None)
    parser.add_argument("--distance-bands", nargs="+", type=float)
    parser.add_argument("--overwrite", action="store_true")
    return parser.parse_args()


def target_counts(total: int, labels: list[str]) -> dict[str, int]:
    base, remainder = divmod(total, len(labels))
    return {label: base + (index < remainder) for index, label in enumerate(labels)}


def pair_row(
    graph: nx.DiGraph,
    mapping: dict[int, int | None],
    origin: int,
    destination: int,
    boundaries: tuple[float, ...],
) -> dict:
    first = graph.nodes[origin]
    second = graph.nodes[destination]
    distance = haversine_km((first["lat"], first["lon"]), (second["lat"], second["lon"]))
    return {
        "origin": origin,
        "destination": destination,
        "origin_osm_id": "" if mapping[origin] is None else mapping[origin],
        "destination_osm_id": "" if mapping[destination] is None else mapping[destination],
        "origin_lat": f"{first['lat']:.6f}",
        "origin_lon": f"{first['lon']:.6f}",
        "destination_lat": f"{second['lat']:.6f}",
        "destination_lon": f"{second['lon']:.6f}",
        "flight_distance_km": f"{distance:.6f}",
        "distance_band": distance_band(distance, boundaries),
    }


def sample_pairs(
    graph: nx.DiGraph,
    mapping: dict[int, int | None],
    count: int,
    seed: int,
    balanced: bool,
    boundaries: tuple[float, ...],
    report: bool = False,
) -> list[dict]:
    components = [tuple(component) for component in nx.strongly_connected_components(graph) if len(component) > 1]
    if not components:
        raise ValueError("the map contains no strongly connected component with at least two nodes")
    possible = sum(len(component) * (len(component) - 1) for component in components)
    if count > possible:
        raise ValueError(f"requested {count} unique pairs, but at most {possible} are available")

    rng = random.Random(seed)
    weighted_components = [component for component in components for _ in range(len(component))]
    labels = [distance_band(value - 1e-12 if value else 0, boundaries) for value in boundaries]
    labels.append(distance_band(boundaries[-1], boundaries))
    targets = target_counts(count, labels) if balanced else {}
    selected: dict[tuple[int, int], dict] = {}
    per_band = {label: 0 for label in labels}
    observed_per_band = {label: 0 for label in labels}
    maximum_attempts = max(100_000, count * 10_000)
    attempts = 0
    evaluated_distances = 0
    distance_total = 0.0
    distance_minimum = float("inf")
    distance_maximum = 0.0

    for _ in range(maximum_attempts):
        if len(selected) == count:
            break
        attempts += 1
        component = rng.choice(weighted_components)
        origin, destination = rng.sample(component, 2)
        key = (origin, destination)
        if key in selected:
            continue
        row = pair_row(graph, mapping, origin, destination, boundaries)
        distance = float(row["flight_distance_km"])
        evaluated_distances += 1
        distance_total += distance
        distance_minimum = min(distance_minimum, distance)
        distance_maximum = max(distance_maximum, distance)
        label = row["distance_band"]
        observed_per_band[label] += 1
        if balanced and per_band[label] >= targets[label]:
            continue
        selected[key] = row
        per_band[label] += 1
    distance_summary = (
        "no candidate distances evaluated"
        if evaluated_distances == 0
        else (
            f"min={distance_minimum:.3f}, mean={distance_total / evaluated_distances:.3f}, "
            f"max={distance_maximum:.3f} km; candidates by band: "
            + ", ".join(
                f"{label}={observed_per_band[label]} "
                f"({observed_per_band[label] / evaluated_distances:.1%})"
                for label in labels
            )
        )
    )
    sampling_summary = (
        f"attempts={attempts}/{maximum_attempts}, "
        f"evaluated={evaluated_distances}, accepted={len(selected)}"
    )
    if len(selected) != count:
        detail = (
            ", ".join(f"{label}={per_band[label]}/{targets[label]}" for label in labels)
            if balanced
            else f"accepted={len(selected)}/{count}"
        )
        raise ValueError(
            f"could not satisfy requested pairs ({detail}; {sampling_summary}; {distance_summary}); "
            "adjust --distance-bands, lower --count, or use --no-balanced"
        )
    if report:
        print(f"Sampling: {sampling_summary}")
        print(f"Candidate flight distance: {distance_summary}")
    rows = list(selected.values())
    for pair_id, row in enumerate(rows):
        row["pair_id"] = pair_id
    return rows


def main() -> int:
    args = parse_args()
    map_path = args.map_path or MAP_HDF5
    output = args.output or OUTPUT_CSV
    count = PAIR_COUNT if args.count is None else args.count
    seed = RANDOM_SEED if args.seed is None else args.seed
    balanced = BALANCE_DISTANCE_BANDS if args.balanced is None else args.balanced
    boundaries = tuple(args.distance_bands or DISTANCE_BANDS_KM)
    if count <= 0:
        raise ValueError("pair count must be positive")
    if not boundaries or any(value <= 0 for value in boundaries) or tuple(sorted(boundaries)) != boundaries:
        raise ValueError("distance bands must be positive and strictly increasing")
    if len(set(boundaries)) != len(boundaries):
        raise ValueError("distance bands must be strictly increasing")
    if output.exists() and not (args.overwrite or OVERWRITE):
        raise FileExistsError(f"refusing to overwrite {output}")

    graph, mapping = read_hdf_graph(map_path)
    rows = sample_pairs(graph, mapping, count, seed, balanced, boundaries, report=True)
    output.parent.mkdir(parents=True, exist_ok=True)
    with output.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=FIELDNAMES)
        writer.writeheader()
        writer.writerows(rows)
    print(f"Saved {len(rows)} reachable pairs to {output} (seed={seed}, balanced={balanced})")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except Exception as error:
        print(f"error: {error}", file=sys.stderr)
        sys.exit(1)
