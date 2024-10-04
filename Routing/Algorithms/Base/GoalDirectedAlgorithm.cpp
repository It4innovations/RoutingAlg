
#include "GoalDirectedAlgorithm.h"

Routing::Algorithms::GoalDirectedAlgorithm::GoalDirectedAlgorithm(std::shared_ptr<GraphMemory> routingGraph,
                                                                  AlgorithmSettings settings,
                                                                  HeuristicTravelCostCalculator *heuristicTravelCostCalculator,
                                                                  TravelTimeCalculator *travelTimeCalculator)
        : StaticRoutingAlgorithm(routingGraph, settings, nullptr, travelTimeCalculator),
          heuristicCostCalculator(heuristicTravelCostCalculator) {}


Routing::Algorithms::HeuristicTravelCostCalculator *
Routing::Algorithms::GoalDirectedAlgorithm::GetHeuristicTravelCostCalculator() {
    return heuristicCostCalculator;
}
