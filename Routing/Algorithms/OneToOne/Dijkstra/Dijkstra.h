#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "../../Base/StaticRoutingAlgorithm.h"
#include "../../../Data/Collections/BinaryHeap.h"
#include "../../../Data/Indexes/GraphMemory.h"
#include "../../../Data/Utility/Definitions.h"
#include <sparsehash/dense_hash_map>
#include "../../Common/VisitedNode.h"


namespace Routing {

    namespace Algorithms {

        class Dijkstra : public StaticRoutingAlgorithm {
        public:
            Dijkstra(std::shared_ptr<GraphMemory> routingGraph, AlgorithmSettings settings = AlgorithmSettings(),
                     TravelCostCalculator *travelCostCalculator = &TravelCostCalculator::Instance(),
                     TravelTimeCalculator *travelTimeCalculator = &TravelTimeCalculator::Instance());

        protected:
            std::vector<Segment> GetRoute(int startId, int endId, CostCalcType costType) override final;

        };
    }
}


