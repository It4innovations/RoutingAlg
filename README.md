# Static Routing Library

RoutingAlg is a C++ static-routing library operating on a custom road-network representation stored in HDF5.

It includes:

- Dijkstra
- Bidirectional Dijkstra
- A-star
- Bidirectional A-star
- Plateau alternative routing
- Probabilistic time-dependent routing

## Alternative routes

The Plateau algorithm calculates several possible routes from an origin to a destination. Geographic and road-hierarchy filters can reduce the search area and their effect can be compared with the unfiltered algorithm.

The historical supporting Python repository is available at:
<https://code.it4i.cz/smo0117/antarex-simple-py>
It provided the original workflows for generating HDF5 maps, selecting origin/destination nodes interactively, and visualizing routes. This repository now contains self-contained scripts for those tasks, described after the quick start.

## Build

The C++ build requires CMake, a C++17 compiler, HDF5 C++ libraries, SQLite, and their development headers.

Configure and build from the repository root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target routing-alt -j
```

Always use a Release build for benchmarks. Debug builds print internal diagnostics and are substantially slower.

## Run one pair with the included map

[`main-alt.cpp`](main-alt.cpp) is intentionally a short, editable example with no
command-line parser. It already uses the included `data/graph.hdf5` map and the
node pair `3916 → 1114`:

```bash
cmake --build build --target routing-alt -j
./build/routing-alt
```

Edit the constants and filter settings directly in that file, rebuild, and run
it again. The executable prints the query time and returned routes to the terminal.

## Timing measurements

All durations use a monotonic clock and fractional milliseconds.

- `query_ms` measures only `AlternativesPlateauAlgorithm::GetResults`.
- Route and CSV serialization happen after the query timer stops.
- `wall_ms` includes the surrounding repetition work.
- Graph loading, OD loading and validation, and algorithm construction are measured separately in `summary.csv`.

Warmups are recorded in `queries.csv` with `phase=warmup` but excluded from the per-profile statistics in `summary.csv`. There is no hidden fallback to the unfiltered algorithm: a profile that finds no route is recorded as `no_route`.

## Create your own map

The Python tools use the Python 3.11 environment.
It provides OSMnx, NetworkX, h5py, NumPy, pandas, and Matplotlib.

[`scripts/build_map.py`](scripts/build_map.py) supports three sources:

- A geocoded OSM place name.
- An explicit north/west/south/east bounding box.
- An existing GraphML file.

It produces matching processed `.graphml` and `.hdf5` files. Roundabout nodes and cycles are retained. If several parallel OSM edges connect the same directed node pair, the shortest is kept for the RoutingAlg graph.

### Command-line examples

```bash
# Existing GraphML
python scripts/build_map.py \
  --graphml data/prague_map.graphml \
  --output data/converted

# Place name
python scripts/build_map.py \
  --place "Prague, Czechia" \
  --output data/prague

# Bounding box: north west south east
python scripts/build_map.py \
  --bbox 50.15 14.20 49.95 14.75 \
  --output data/prague-bbox
```

Existing outputs are protected unless `--overwrite` is supplied or `OVERWRITE` is enabled. Downloading place or bounding-box data requires network access and may take time for large areas.
For a reusable default setup, edit the configuration block near the top of `scripts/build_map.py`. Leave exactly one source enabled.

### OSM node IDs

RoutingAlg uses compact signed 32-bit node IDs, while modern OSM node IDs may be much larger. Generated maps retain the relationship in both formats:

- GraphML nodes have a `routing_node_id` attribute.
- HDF5 contains `/Index/OsmNodeMap`, mapping routing IDs to original 64-bit OSM IDs.

This makes it possible to translate and plot every intermediate node of a calculated route.

## Generate an OD file

[`scripts/generate_od.py`](scripts/generate_od.py) samples unique directed pairs that are guaranteed to be reachable. It records both RoutingAlg and OSM IDs, coordinates, Haversine flight distance, and distance-band metadata.

Run the configured values from the file with:

```bash
python scripts/generate_od.py
```

Or provide values on the command line:

```bash
# Reachable random pairs without distance quotas
python scripts/generate_od.py \
  --map data/converted.hdf5 \
  --output data/converted-od-random.csv \
  --count 250 \
  --seed 2026 \
  --no-balanced

# Balanced by flight distance
python scripts/generate_od.py \
  --map data/converted.hdf5 \
  --output data/converted-od.csv \
  --count 250 \
  --seed 2026 \
  --balanced
```

Balanced mode divides the requested count as evenly as possible across `<3`, `3–15`, `15–50`, `50–120`, and `>120` km flight-distance bands. Change them with `--distance-bands`. If a map cannot provide enough pairs in a requested band, the script reports the achieved counts and stops instead of writing a biased partial file.

The OD CSV schema is:

```text
pair_id,origin,destination,origin_osm_id,destination_osm_id,
origin_lat,origin_lon,destination_lat,destination_lon,
flight_distance_km,distance_band
```

Only `pair_id`, `origin`, and `destination` are required by the C++ reader. Legacy HDF5 maps without `/Index/OsmNodeMap` remain usable, but their OSM-ID fields are blank.

## Benchmark an OD file

One benchmark invocation loads one map and one OD file:

```bash
cmake --build build --target routing-alt-benchmark -j
./build/routing-alt-benchmark \
  --map data/converted.hdf5 \
  --od data/converted-od.csv \
  --profiles none,ellipse,hierarchy,ellipse-hierarchy \
  --warmups 1 \
  --repetitions 1 \
  --max-routes 10 \
  --output-dir results/converted-balanced
```

The benchmark writes:

| File | Contents |
| --- | --- |
| `queries.csv` | Every warmup and measured query, including duration and status. |
| `routes.csv` | One summary row for every measured alternative route. |
| `route-nodes.csv` | Ordered route nodes when `--export-paths` is enabled. |
| `profiles.csv` | Exact filter and hierarchy settings used by the run. |
| `summary.csv` | Per-repetition totals and per-profile timing statistics. |

Bulk node-sequence output is disabled by default to reduce output size. Enable it when needed:

```bash
./build/routing-alt-benchmark \
  --map data/prague.hdf5 \
  --od data/prague-od.csv \
  --output-dir results/prague-with-paths \
  --export-paths
```

## Configure filter profiles

The short [`main-alt.cpp`](main-alt.cpp) exposes one filter configuration directly.
The benchmark tool defines its named profiles in `BuiltInProfiles()` in
[`main-alt-benchmark.cpp`](main-alt-benchmark.cpp).

The built-in profiles are:

| Profile | Geometry filter | Hierarchy filter |
| --- | --- | --- |
| `none` | Off | Off |
| `ellipse` | Ellipse | Off |
| `hierarchy` | Off | Default hierarchy |
| `ellipse-hierarchy` | Ellipse | Default hierarchy |

A commented custom example exposes the full hierarchy configuration. Copy it,
assign a unique name, edit its FRC and distance values, add it to `profiles`, and
rebuild `routing-alt-benchmark`.

List the profiles compiled into the benchmark executable with:

```bash
./build/routing-alt-benchmark --list-profiles
```

Important fields include:

- `minFRC` and `maxFRC`: initial allowed functional-road-class range.
- `firstHierarchyJumpFRC` and `secondHierarchyJumpFRC`: limits after the two distance boundaries.
- `firstHierarchyJumpDistance` and `secondHierarchyJumpDistance`: boundaries for normal trips.
- `firstHierarchyJumpLongDistance` and `secondHierarchyJumpLongDistance`: boundaries for trips longer than `longDistanceValue`.

To experiment with one route, edit and run the small example:

```bash
cmake --build build --target routing-alt -j
./build/routing-alt
```

Benchmark runs write the effective values to `profiles.csv`, so results retain
their exact configuration.

## Plot routes

Use the processed GraphML map and a populated `route-nodes.csv`:

```bash
python scripts/plot_routes.py \
  --graphml data/prague.graphml \
  --routes results/prague-with-paths/route-nodes.csv \
  --output results/prague-with-paths/routes.png
```

Single-pair mode always writes route nodes. Bulk mode writes them only with `--export-paths`. Plotting through OSM IDs requires a map generated by `scripts/build_map.py`; legacy maps remain routable but lack that mapping.

## HDF5 map structure

```text
/Index
  attributes: PartsCount, CreationTime, PartsInfo, Source,
              RoundaboutsContracted=false
  /CZE
    Nodes     (id, latitudeInt, longitudeInt, edgeOutCount,
               edgeOutIndex, edgeInCount)
    Edges     (edgeId, nodeIndex, computed_speed, length, edgeDataIndex)
  EdgeData    (speed, funcClass, lanes, access and vehicle constraints)
  NodeMap     (nodeId, partId, nodeIndex)
  OsmNodeMap  (nodeId, osmNodeId)
```

`OsmNodeMap` and the additional attributes are additive; the routing library ignores unknown HDF5 members. Edge-data records are deduplicated because their C++ index is `uint16`. Node in/out degree is stored as `uint8`; map creation reports a clear error if a graph exceeds that format limit.
