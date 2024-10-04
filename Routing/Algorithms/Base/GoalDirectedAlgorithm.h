#pragma once

#include "StaticRoutingAlgorithm.h"
#include "../../Data/Utility/Constants.h"
#include "../TravelCostCalculators/HeuristicTravelCostCalculator.h"
#include "../TravelCostCalculators/TravelTimeCalculator.h"
#include "../Common/Result.h"
#include "../Common/Segment.h"
#include "../../Data/Utility/TimeWatch.h"

namespace Routing {

    namespace Algorithms {
        class GoalDirectedAlgorithm : public StaticRoutingAlgorithm {
        public:
            GoalDirectedAlgorithm(std::shared_ptr<GraphMemory> routingGraph,
                                  AlgorithmSettings settings = AlgorithmSettings(),
                                  HeuristicTravelCostCalculator *travelCostCalculator = &HeuristicTravelCostCalculator::Instance(),
                                  TravelTimeCalculator *travelTimeCalculator =
                                  &TravelTimeCalculator::Instance());

            HeuristicTravelCostCalculator *GetHeuristicTravelCostCalculator();

            float GetHeuristicFactor(void) { return this->heuristicFactor; };

        protected:
            HeuristicTravelCostCalculator *heuristicCostCalculator;

        private:
            float heuristicFactor = Constants::HeuristicFactor;
        };
    };
}



