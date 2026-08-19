#!/usr/bin/env python3
"""Download or load an OSMnx map and write matching GraphML/HDF5 files."""

from __future__ import annotations

import argparse
from datetime import datetime, timezone
from pathlib import Path
import sys

import networkx as nx
import osmnx as ox

from routing_hdf import normalize_graph, save_graph_to_hdf5


# Editable no-argument configuration. Leave exactly one source non-None.
PLACE = "Prague, Czechia"
BBOX = None  # Example: (50.15, 14.20, 49.95, 14.75) = north, west, south, east
GRAPHML_INPUT = None  # Example: Path("data/prague-source.graphml")
OUTPUT_STEM = Path("data/prague")
OVERWRITE = False

CUSTOM_DRIVE_FILTER = (
    '["highway"]["area"!~"yes"]["access"!~"private"]'
    '["highway"!~"abandoned|bridleway|bus_guideway|corridor|cycleway|elevator|'
    'escalator|footway|no|path|pedestrian|planned|platform|proposed|raceway|razed|'
    'steps|track|closed"]["motor_vehicle"!~"no"]["motorcar"!~"no"]'
    '["service"!~"alley|driveway|emergency_access|parking|parking_aisle|private"]'
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    source = parser.add_mutually_exclusive_group()
    source.add_argument("--place", help="OSM place name to geocode and download")
    source.add_argument(
        "--bbox",
        nargs=4,
        type=float,
        metavar=("NORTH", "WEST", "SOUTH", "EAST"),
        help="bounding box to download",
    )
    source.add_argument("--graphml", type=Path, help="existing GraphML input")
    parser.add_argument("--output", type=Path, help="output stem without extension")
    parser.add_argument("--overwrite", action="store_true", help="replace existing outputs")
    return parser.parse_args()


def resolve_config(args: argparse.Namespace) -> tuple[str, object, Path, bool]:
    cli_sources = [("place", args.place), ("bbox", args.bbox), ("graphml", args.graphml)]
    selected_cli = [(kind, value) for kind, value in cli_sources if value is not None]
    if selected_cli:
        kind, value = selected_cli[0]
    else:
        configured = [("place", PLACE), ("bbox", BBOX), ("graphml", GRAPHML_INPUT)]
        selected = [(source_kind, source_value) for source_kind, source_value in configured if source_value is not None]
        if len(selected) != 1:
            raise ValueError("configure exactly one of PLACE, BBOX, or GRAPHML_INPUT")
        kind, value = selected[0]
    output = args.output if args.output is not None else OUTPUT_STEM
    return kind, value, output, args.overwrite or OVERWRITE


def load_source(kind: str, value: object) -> nx.Graph:
    if kind == "place":
        print(f"Downloading drive network for {value!r} ...")
        return ox.graph_from_place(
            str(value), network_type="drive", retain_all=False, custom_filter=CUSTOM_DRIVE_FILTER
        )
    if kind == "bbox":
        north, west, south, east = value
        if not north > south or not east > west:
            raise ValueError("bbox must satisfy north > south and east > west")
        print(f"Downloading drive network for bbox {value} ...")
        return ox.graph_from_bbox(
            bbox=(west, south, east, north),
            network_type="drive",
            retain_all=False,
            custom_filter=CUSTOM_DRIVE_FILTER,
        )
    path = Path(value)
    if not path.is_file():
        raise FileNotFoundError(f"GraphML input does not exist: {path}")
    print(f"Loading {path} ...")
    return ox.load_graphml(path)


def main() -> int:
    args = parse_args()
    kind, value, output_stem, overwrite = resolve_config(args)
    graphml_path = output_stem.with_suffix(".graphml")
    hdf5_path = output_stem.with_suffix(".hdf5")
    existing = [path for path in (graphml_path, hdf5_path) if path.exists()]
    if existing and not overwrite:
        raise FileExistsError(f"refusing to overwrite: {', '.join(map(str, existing))}")
    output_stem.parent.mkdir(parents=True, exist_ok=True)

    graph = load_source(kind, value)
    if any("speed_kph" not in data for _, _, data in graph.edges(data=True)):
        if graph.is_multigraph():
            existing_speeds = {
                (origin, destination, key): data["speed_kph"]
                for origin, destination, key, data in graph.edges(keys=True, data=True)
                if "speed_kph" in data
            }
        else:
            existing_speeds = {
                (origin, destination): data["speed_kph"]
                for origin, destination, data in graph.edges(data=True)
                if "speed_kph" in data
            }
        graph = ox.add_edge_speeds(graph, fallback=50)
        nx.set_edge_attributes(graph, existing_speeds, "speed_kph")
    graph = normalize_graph(graph)
    graph.graph.update(
        {
            "routing_source": f"{kind}:{value}",
            "routing_created_utc": datetime.now(timezone.utc).isoformat(),
            "routing_osmnx_version": ox.__version__,
        }
    )

    # GraphML and HDF5 intentionally represent the same processed topology.
    ox.save_graphml(nx.MultiDiGraph(graph), filepath=graphml_path)
    save_graph_to_hdf5(graph, hdf5_path, source=f"{kind}:{value}")
    print(f"Saved {graph.number_of_nodes()} nodes and {graph.number_of_edges()} edges")
    print(f"GraphML: {graphml_path}")
    print(f"HDF5:   {hdf5_path}")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except Exception as error:  # concise command-line failure instead of a long library traceback
        print(f"error: {error}", file=sys.stderr)
        sys.exit(1)
