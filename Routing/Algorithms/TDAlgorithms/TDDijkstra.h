#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sparsehash/dense_hash_map>

#include "TDBase.h"
#include "TDUtility.h"
#include "../../Data/Collections/BinaryHeap.h"
#include "../../Data/Indexes/GraphMemory.h"
#include "../../Data/Utility/Definitions.h"
#include "../Common/Segment.h"
#include "../Common/Result.h"
#include "../Common/AlgorithmHelper.h"
#include "../Common/VisitedNode.h"
#include "../TravelCostCalculators/TravelCostCalculator.h"
#include "../TravelCostCalculators/TravelTimeCalculator.h"
#include "../TravelCostCalculators/CostCalculator.h"
#include "../GraphFilter/GraphFilterGeometry.h"


#include "../Probability/Helpers/Utility.h"


namespace Routing {

    namespace Algorithms {

        namespace TD {

            class TDDijkstra : TDBase {
            public:
                TDDijkstra(std::shared_ptr<GraphMemory> routingGraph,
                           std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5> storage,
                           AlgorithmSettings settings = AlgorithmSettings()) :
                        TDBase(routingGraph, storage, settings) {
                };

                std::vector<Routing::Algorithms::Segment> GetRoute(int startId, int endId, int startTime);

                std::unique_ptr<Routing::Algorithms::Result> GetResult(int startId, int endId, int startTime);

            protected:
                Routing::Algorithms::GraphFilterGeometry InitiateFilters(GraphFilterGeometry::FilterType typeOfFilter,
                                                                         Node startNode, Node endNode);

                void FindPath(int endId, google::dense_hash_map<int, VisitedNode> &closedSet, BinHeap &openSet,
                              Routing::Algorithms::GraphFilterGeometry &filterRoot);

            };
        }
    }
}

