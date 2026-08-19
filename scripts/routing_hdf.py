"""Shared HDF5 map helpers for the RoutingAlg command-line tools."""

from __future__ import annotations

from collections import Counter, OrderedDict
from datetime import datetime, timezone
import math
from pathlib import Path
import re
from typing import Iterable

import h5py
import networkx as nx
import numpy as np


PART_NAME = b"CZE"
ROUTING_ID_OFFSET = 1000
UINT8_MAX = np.iinfo(np.uint8).max
UINT16_MAX = np.iinfo(np.uint16).max
INT32_MAX = np.iinfo(np.int32).max

HIGHWAY_TO_FUNC_CLASS = {
    "motorway": 0,
    "motorway_link": 0,
    "trunk": 1,
    "trunk_link": 1,
    "primary": 2,
    "primary_link": 2,
    "secondary": 3,
    "secondary_link": 3,
    "tertiary": 4,
    "tertiary_link": 4,
    "unclassified": 5,
    "residential": 6,
    "living_street": 7,
    "service": 7,
}

NODE_DTYPE = np.dtype(
    [
        ("id", np.int32),
        ("latitudeInt", np.int32),
        ("longitudeInt", np.int32),
        ("edgeOutCount", np.uint8),
        ("edgeOutIndex", np.int32),
        ("edgeInCount", np.uint8),
    ]
)
EDGE_DTYPE = np.dtype(
    [
        ("edgeId", np.int32),
        ("nodeIndex", np.int32),
        ("computed_speed", np.int32),
        ("length", np.int32),
        ("edgeDataIndex", np.uint16),
    ]
)
EDGE_DATA_DTYPE = np.dtype(
    [
        ("id", np.int32),
        ("speed", np.uint8),
        ("funcClass", np.uint8),
        ("lanes", np.uint8),
        ("vehicleAccess", np.uint8),
        ("specificInfo", np.uint16),
        ("maxWeight", np.uint16),
        ("maxHeight", np.uint16),
        ("maxAxleLoad", np.uint8),
        ("maxWidth", np.uint8),
        ("maxLength", np.uint8),
        ("incline", np.int8),
    ]
)
NODE_MAP_DTYPE = np.dtype(
    [("nodeId", np.int32), ("partId", "S4"), ("nodeIndex", np.int32)]
)
OSM_NODE_MAP_DTYPE = np.dtype(
    [("nodeId", np.int32), ("osmNodeId", np.int64)]
)
PARTS_INFO_DTYPE = np.dtype(
    [("id", "S4"), ("nodeCount", np.int64), ("edgeCount", np.int64)]
)


def _as_values(value: object) -> list[str]:
    if isinstance(value, (list, tuple, set)):
        return [str(item) for item in value]
    if value is None:
        return []
    return [item.strip() for item in str(value).split(";")]


def _first_number(value: object, default: float) -> float:
    for item in _as_values(value):
        match = re.search(r"[-+]?\d+(?:\.\d+)?", item)
        if match:
            return float(match.group(0))
    return default


def edge_speed_kph(data: dict) -> int:
    raw = data.get("speed_kph", data.get("maxspeed", 50))
    speed = int(round(_first_number(raw, 50.0)))
    if not 1 <= speed <= UINT8_MAX:
        raise ValueError(f"edge speed {speed} km/h does not fit uint8 or is not positive")
    return speed


def edge_func_class(data: dict) -> int:
    classes = [
        HIGHWAY_TO_FUNC_CLASS[value.strip().lower()]
        for value in _as_values(data.get("highway"))
        if value.strip().lower() in HIGHWAY_TO_FUNC_CLASS
    ]
    return min(classes, default=7)


def edge_lanes(data: dict) -> int:
    values = [int(number) for number in re.findall(r"[-+]?\d+", str(data.get("lanes", 1)))]
    lanes = max((value for value in values if value > 0), default=1)
    oneway = str(data.get("oneway", "false")).lower() in {"true", "1", "yes"}
    if not oneway and lanes > 1:
        lanes = max(1, (lanes + 1) // 2)
    if not 1 <= lanes <= UINT8_MAX:
        raise ValueError(f"edge lane count {lanes} does not fit uint8")
    return lanes


def edge_specific_info(data: dict) -> int:
    value = int(data.get("specificInfo", 0))
    if str(data.get("junction", "")).lower() == "roundabout":
        value |= 8
    if str(data.get("toll", "")).lower() in {"true", "1", "yes"}:
        value |= 1
    if not 0 <= value <= UINT16_MAX:
        raise ValueError(f"edge specificInfo={value} does not fit uint16")
    return value


def _bounded_int(data: dict, name: str, default: int, low: int, high: int) -> int:
    value = int(data.get(name, default))
    if not low <= value <= high:
        raise ValueError(f"edge {name}={value} is outside [{low}, {high}]")
    return value


def normalize_graph(graph: nx.Graph) -> nx.DiGraph:
    """Return the exact directed graph written to GraphML and HDF5.

    Parallel OSM ways are reduced to the shortest edge, matching RUTH's use of
    ``osmnx.get_digraph``. Roundabout nodes and edges are deliberately retained.
    """
    if graph.number_of_nodes() == 0:
        raise ValueError("the input graph is empty")
    if graph.number_of_edges() == 0:
        raise ValueError("the input graph has no edges")

    if graph.is_multigraph():
        normalized = nx.DiGraph()
        normalized.graph.update(graph.graph)
        normalized.add_nodes_from((node, dict(data)) for node, data in graph.nodes(data=True))
        for node_from, node_to, data in graph.edges(data=True):
            length = float(data.get("length", math.inf))
            current = normalized.get_edge_data(node_from, node_to)
            if current is None or length < float(current.get("length", math.inf)):
                normalized.add_edge(node_from, node_to, **dict(data))
    elif graph.is_directed():
        normalized = nx.DiGraph(graph)
    else:
        normalized = nx.DiGraph(graph.to_directed())

    relabeling = {}
    for node_id in normalized.nodes:
        try:
            integer_id = int(node_id)
        except (TypeError, ValueError) as error:
            raise ValueError(f"OSM node ID {node_id!r} is not an integer") from error
        if integer_id != node_id:
            relabeling[node_id] = integer_id
    if len(set(int(node_id) for node_id in normalized.nodes)) != normalized.number_of_nodes():
        raise ValueError("OSM node IDs are not unique after integer conversion")
    if relabeling:
        normalized = nx.relabel_nodes(normalized, relabeling, copy=True)

    ordered_nodes = list(normalized.nodes())
    if len(ordered_nodes) + ROUTING_ID_OFFSET - 1 > INT32_MAX:
        raise ValueError("the graph has too many nodes for RoutingAlg int32 IDs")

    osm_to_routing: dict[int, int] = {}
    for index, osm_node_id in enumerate(ordered_nodes):
        routing_id = index + ROUTING_ID_OFFSET
        osm_to_routing[osm_node_id] = routing_id
        node = normalized.nodes[osm_node_id]
        if "x" not in node or "y" not in node:
            raise ValueError(f"node {osm_node_id} is missing x/y coordinates")
        node["routing_node_id"] = routing_id

    if normalized.number_of_edges() > INT32_MAX:
        raise ValueError("the graph has too many edges for RoutingAlg int32 IDs")
    for edge_id, (_, _, data) in enumerate(normalized.edges(data=True), start=1):
        data["routing_id"] = edge_id
        raw_length = data.get("length")
        try:
            length = float(raw_length)
        except (TypeError, ValueError) as error:
            raise ValueError(
                f"edge {edge_id} has invalid source length {raw_length!r}"
            ) from error
        if not math.isfinite(length) or length <= 0 or length > INT32_MAX:
            raise ValueError(f"edge {edge_id} has invalid source length {length}")
        data["length"] = max(1, int(round(length)))
        data["speed_kph"] = edge_speed_kph(data)
        data["lanes"] = edge_lanes(data)

    normalized.graph["routing_roundabouts_contracted"] = False
    normalized.graph["routing_id_offset"] = ROUTING_ID_OFFSET
    return normalized


def _edge_data_values(data: dict) -> tuple[int, ...]:
    return (
        edge_speed_kph(data),
        edge_func_class(data),
        edge_lanes(data),
        _bounded_int(data, "vehicleAccess", 1, 0, UINT8_MAX),
        edge_specific_info(data),
        _bounded_int(data, "maxWeight", UINT16_MAX, 0, UINT16_MAX),
        _bounded_int(data, "maxHeight", UINT16_MAX, 0, UINT16_MAX),
        _bounded_int(data, "maxAxleLoad", UINT8_MAX, 0, UINT8_MAX),
        _bounded_int(data, "maxWidth", UINT8_MAX, 0, UINT8_MAX),
        _bounded_int(data, "maxLength", UINT8_MAX, 0, UINT8_MAX),
        _bounded_int(data, "incline", 0, -128, 127),
    )


def save_graph_to_hdf5(graph: nx.DiGraph, output_path: Path, source: str) -> dict[int, int]:
    output_path = Path(output_path)
    node_ids = list(graph.nodes())
    node_index = {node_id: index for index, node_id in enumerate(node_ids)}
    osm_to_routing = {
        int(node_id): int(graph.nodes[node_id]["routing_node_id"]) for node_id in node_ids
    }

    edge_data_indexes: OrderedDict[tuple[int, ...], int] = OrderedDict()
    for _, _, data in graph.edges(data=True):
        values = _edge_data_values(data)
        if values not in edge_data_indexes:
            edge_data_indexes[values] = len(edge_data_indexes)
    if len(edge_data_indexes) > UINT16_MAX + 1:
        raise ValueError(
            f"{len(edge_data_indexes)} unique edge-data records exceed the uint16 index limit"
        )

    nodes: list[tuple[int, ...]] = []
    edges: list[tuple[int, ...]] = []
    edge_offset = 0
    for osm_node_id in node_ids:
        data = graph.nodes[osm_node_id]
        out_edges = list(graph.out_edges(osm_node_id, data=True))
        in_count = graph.in_degree(osm_node_id)
        if len(out_edges) > UINT8_MAX or in_count > UINT8_MAX:
            raise ValueError(
                f"node {osm_node_id} degree exceeds the HDF5 uint8 limit "
                f"(out={len(out_edges)}, in={in_count})"
            )
        nodes.append(
            (
                osm_to_routing[int(osm_node_id)],
                int(round(float(data["y"]) * 1_000_000)),
                int(round(float(data["x"]) * 1_000_000)),
                len(out_edges),
                edge_offset,
                in_count,
            )
        )
        for _, destination, edge in out_edges:
            values = _edge_data_values(edge)
            edges.append(
                (
                    int(edge["routing_id"]),
                    node_index[destination],
                    edge_speed_kph(edge),
                    int(edge["length"]),
                    edge_data_indexes[values],
                )
            )
        edge_offset += len(out_edges)

    edge_data = [
        (index, *values) for values, index in edge_data_indexes.items()
    ]
    node_map = [
        (osm_to_routing[int(osm_id)], PART_NAME, index)
        for index, osm_id in enumerate(node_ids)
    ]
    osm_node_map = [
        (osm_to_routing[int(osm_id)], int(osm_id)) for osm_id in node_ids
    ]

    node_array = np.asarray(nodes, dtype=NODE_DTYPE).reshape((-1, 1))
    edge_array = np.asarray(edges, dtype=EDGE_DTYPE).reshape((-1, 1))
    edge_data_array = np.asarray(edge_data, dtype=EDGE_DATA_DTYPE).reshape((-1, 1))
    node_map_array = np.asarray(node_map, dtype=NODE_MAP_DTYPE).reshape((-1, 1))
    osm_node_map_array = np.asarray(osm_node_map, dtype=OSM_NODE_MAP_DTYPE).reshape((-1, 1))

    with h5py.File(output_path, "w") as handle:
        index_group = handle.create_group("Index")
        index_group.attrs["PartsCount"] = 1
        index_group.attrs["CreationTime"] = int(datetime.now(timezone.utc).timestamp())
        index_group.attrs["PartsInfo"] = np.asarray(
            [(PART_NAME, len(nodes), len(edges))], dtype=PARTS_INFO_DTYPE
        )
        index_group.attrs["Source"] = source
        index_group.attrs["RoundaboutsContracted"] = False

        part_group = index_group.create_group(PART_NAME.decode())
        part_group.attrs["PartInfo"] = PART_NAME
        part_group.create_dataset("Nodes", data=node_array, compression="gzip", compression_opts=4)
        part_group.create_dataset("Edges", data=edge_array, compression="gzip", compression_opts=4)
        index_group.create_dataset("EdgeData", data=edge_data_array, compression="gzip", compression_opts=4)
        index_group.create_dataset("NodeMap", data=node_map_array, compression="gzip", compression_opts=4)
        index_group.create_dataset(
            "OsmNodeMap", data=osm_node_map_array, compression="gzip", compression_opts=4
        )

    histogram = Counter(values[1] for values in edge_data_indexes)
    print(
        "FRC histogram: "
        + ", ".join(f"{frc}={histogram[frc]}" for frc in range(8))
    )
    return osm_to_routing


def read_hdf_graph(path: Path) -> tuple[nx.DiGraph, dict[int, int | None]]:
    """Read the RoutingAlg graph and optional routing-to-OSM mapping."""
    with h5py.File(path, "r") as handle:
        index = handle["Index"]
        part_names = [name for name in index if isinstance(index[name], h5py.Group)]
        if len(part_names) != 1:
            raise ValueError(f"expected exactly one graph part, found {part_names}")
        part = index[part_names[0]]
        nodes = part["Nodes"][:].reshape(-1)
        edges = part["Edges"][:].reshape(-1)
        osm_mapping: dict[int, int | None] = {int(row["id"]): None for row in nodes}
        if "OsmNodeMap" in index:
            for row in index["OsmNodeMap"][:].reshape(-1):
                osm_mapping[int(row["nodeId"])] = int(row["osmNodeId"])

    graph = nx.DiGraph()
    for row in nodes:
        graph.add_node(
            int(row["id"]),
            lat=float(row["latitudeInt"]) / 1_000_000,
            lon=float(row["longitudeInt"]) / 1_000_000,
        )
    for row in nodes:
        origin = int(row["id"])
        start = int(row["edgeOutIndex"])
        count = int(row["edgeOutCount"])
        for edge in edges[start : start + count]:
            destination_index = int(edge["nodeIndex"])
            destination = int(nodes[destination_index]["id"])
            graph.add_edge(origin, destination, length=int(edge["length"]))
    return graph, osm_mapping


def haversine_km(first: tuple[float, float], second: tuple[float, float]) -> float:
    lat1, lon1 = map(math.radians, first)
    lat2, lon2 = map(math.radians, second)
    dlat = lat2 - lat1
    dlon = lon2 - lon1
    value = math.sin(dlat / 2) ** 2 + math.cos(lat1) * math.cos(lat2) * math.sin(dlon / 2) ** 2
    return 6371.0088 * 2 * math.atan2(math.sqrt(value), math.sqrt(1 - value))


def distance_band(distance_km: float, boundaries: Iterable[float]) -> str:
    limits = list(boundaries)
    previous = 0.0
    for limit in limits:
        if distance_km < limit:
            return f"{previous:g}-{limit:g}km"
        previous = limit
    return f"{previous:g}km+"
