#pragma once

#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <sparsehash/dense_hash_map>

#include "TDDijkstra.h"

namespace Routing {

    namespace Algorithms {

        namespace TD {

            class TDDAlternativesUtility : TDBase {
            public:
                TDDAlternativesUtility(std::shared_ptr<GraphMemory> routingGraph,
                                       std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5> storage,
                                       AlgorithmSettings settings = AlgorithmSettings()) :
                        TDBase(routingGraph, storage, settings) {

                    this->dijkstraTD = new Routing::Algorithms::TD::TDDijkstra(routingGraph, storage);
                };

                std::vector<std::vector<Routing::Algorithms::Segment>>
                GetRoute(google::dense_hash_set<int> betweennessNodes,
                         std::vector<std::vector<int>> alternativesNodes,
                         int startId, int endId, int startTime);

                ~TDDAlternativesUtility() {
                    delete (this->dijkstraTD);
                }


            protected:
                Routing::Algorithms::TD::TDDijkstra *dijkstraTD;

                bool NodeInVector(std::vector<int> &middleNodes, int node);

                std::vector<int> FindMiddleNodes(google::dense_hash_set<int> &betweennessNodes,
                                                 std::vector<std::vector<int>> &alternativesNodes, int startId);

                std::vector<std::vector<Routing::Algorithms::Segment>> FindRoutes(std::vector<int> &middleNodes,
                                                                                  int startId, int endId,
                                                                                  int startTime);

                bool IsUniqueRoute(std::vector<std::vector<Routing::Algorithms::Segment>> &results,
                                   std::vector<Routing::Algorithms::Segment> newRoute);


            };
        }
    }
}

