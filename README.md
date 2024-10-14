# Static Routing library
The static routing library contains a set of routing algorithms which operate on custom graph representation of the road network stored in HDF5 file.

## Algorithms
- Dijkstra
- Bidirectional Dijkstra
- A-star
- Bidirectional A-star

- Plateau (Alternatives)

- Probabilistic Time-Dependent Routing


# Alternatives 

- optimized routing algoritm (Plateau Algorithm) for alternative paths
- as result presents several possible routes from point A to B

Supporting python repository:
https://code.it4i.cz/smo0117/antarex-simple-py

- generating base map into hdf5 format
- interactive selecting origin/destination nodes
- visualization of the resulting routes

## Build
    cd routing/graph-algorithms
    mkdir build
    cd build
    cmake ..

## Prepare
In the file `main-alt.cpp` change:
```cpp
// Path to the file with base map
Routing::Tests::LoadGraph("../data/graph.hdf5");

// Set how many % you want your routes to be similar
settings.SetSimilarity(55);

// Origin and destination node id and number of routes
int id_origin = 3916;
int id_destination = 1114;
int max_routes = 10

// Path to the file with result node ids of paths
std::ofstream output("../data/routes.csv");
```

## Compile and Run
    make routing-alt
    ./routing-alt

### Structure of HDF5 map file
````
    Group: Index
    - attr: PartsCount
    - attr: CreationTime    datetime
    - attr: PartsInfo       list of tuples (part_name, number_of_nodes, number_of_edges)

    - group: "PART_NAME"
        - attr: PartInfo    part name
        - dataset: Nodes
            (id, latitudeInt, longitudeInt, edgeOutCount, edgeOutIndex, edgeInCount)
        - dataset: Edges
            (edgeId, nodeIndex, computed_speed, length, edgeDataIndex)
        - dataset: EdgeData
            (id, speed, funcClass, lanes, vehicleAccess, specificInfo,
            maxWeight, maxHeight, maxAxleLoad, maxWidth, maxLength, incline)
        - dataset: NodeMap
            (nodeId, partId, nodeIndex)
````