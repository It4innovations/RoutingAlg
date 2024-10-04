#pragma once

#include <unordered_map>

#include "../../Base/GoalDirectedAlgorithm.h"
#include "../../../Data/Collections/BinaryHeap.h"
#include "../../../Data/Utility/DistanceCalculator.h"
#include "../../Common/VisitedNode.h"
#include "../../Common/AlgorithmHelper.h"
#include "../../Common/Segment.h"
#include "../../../Data/Utility/Definitions.h"

using Routing::Algorithms::Segment;

namespace Routing {

    namespace Algorithms {

        class Astar : public GoalDirectedAlgorithm {
        public:
            Astar(std::shared_ptr<GraphMemory> routingGraph, AlgorithmSettings settings = AlgorithmSettings(),
                  HeuristicTravelCostCalculator *travelCostCalculator = &HeuristicTravelCostCalculator::Instance(),
                  TravelTimeCalculator *travelTimeCalculator =
                  &TravelTimeCalculator::Instance());

        protected:
            std::vector<Segment> GetRoute(int startId, int endId, CostCalcType costType) override final;
        };
    }
}


