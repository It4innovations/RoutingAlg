#include "DistanceMatrix.h"
#include "OneToManyDijkstra.h"
#include "../Probability/Data/CSVReader.h"
#include <vector>
#include <algorithm>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <H5Cpp.h>

using namespace std;
using namespace Routing;

#define CSV_SEPARATOR ';'
#define CSV_ID_POS 0
#define CSV_NODE_POS 3
#define CSV_OUT_HEADER "node_id_from;node_id_to;travel_time;travel_distance"

void Routing::DistanceMatrix::ComputeDistanceMatrix(const string &inputFile, const string &outputFile, const Pipeline::PipelineConfig &config) {

    // Load graph
    H5::H5File h5file(config.routingIndexPath, H5F_ACC_RDONLY);
    unique_ptr<DataIndex> dif(new DataIndex(h5file));
    h5file.close();
    shared_ptr<Data::GraphMemory> routingIndex(new Data::GraphMemory(*dif));

    // Load file
    vector<int> nodes;
    map<int, vector<string>> locationIds; // Key: Node ID, Value: Parsed ID of location

    long lines = 0;

    ifstream inputStream(inputFile);
    Probability::CSVReader reader(CSV_SEPARATOR);

    while(inputStream.good()) {
        inputStream >> reader;
        lines++;

        try {
            int nodeId = stoi(reader[CSV_NODE_POS]);
            string locationId = reader[CSV_ID_POS];

            if(locationIds.find(nodeId) == locationIds.end()) {
                locationIds.emplace(nodeId, initializer_list<string>{locationId});
            } else {
                locationIds[nodeId].push_back(locationId);
            }

            // Check if this node exists in the graph
            if(routingIndex->ContainsNode(nodeId) && find(nodes.begin(), nodes.end(), nodeId) == nodes.end()) {
                nodes.push_back(nodeId);
            }
        }
        catch(invalid_argument &e) {
            cerr << "Invalid number at line " << lines << " " << e.what() << endl;
        }
        catch(out_of_range &e) {
            cerr << "Number out of range at line: " << lines << " " << e.what() << endl;
        }
    }

    inputStream.close();

    // Compute distance matrix
    Algorithms::OneToManyDijkstra distanceAlgorithm(routingIndex);
    distanceAlgorithm.GetAlgorithmSettings().SetFilterType(Algorithms::GraphFilterGeometry::FilterType::NotSet);

    vector<unique_ptr<Routing::Algorithms::DistanceResult>> distances(nodes.size());

    #pragma omp parallel for
    for(size_t i = 0; i < nodes.size(); i++)
    {
        // Take current node and compute distance matrix
        distances[i] = distanceAlgorithm.GetDistances(nodes[i], nodes);
    }

    cout << "Saving results" << endl;
    ofstream outputStream(outputFile);
    if(!outputStream.good()) {
        cerr << "ERROR: Cannot open output file for writing: " << outputFile << endl;
        return;
    }

    outputStream << CSV_OUT_HEADER << endl;
    for(const auto& i : distances)
    {
        for(const auto& d : *i->distances)
        {
            for (const auto &from : locationIds[d.NodeId1])
            {
                for (const auto &to : locationIds[d.NodeId2])
                {
                    outputStream << from << ";" << to << ";" << d.TravelTime << ";" << d.TravelLength << endl;
                }
            }
        }
    }
    outputStream.close();
}
