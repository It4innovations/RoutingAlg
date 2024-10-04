#include "AlternativesAlgorithm.h"

std::unique_ptr<std::vector<Result>> Routing::Algorithms::AlternativesAlgorithm::GetResults(
        int startId, int endId, unsigned int maxRoutes, bool multiThreading, int startTime) {

    TimeWatch watch(true);

    auto alternatives = this->GetRoutes(startId, endId, maxRoutes, multiThreading, startTime);

    if (alternatives.empty()) {
        return nullptr;
    }

    watch.Stop();

    long baseTime = watch.GetElapsed();

    auto *results = new std::vector<Result>();
    results->reserve(alternatives.size());

    for (auto result : alternatives) {
        results->push_back(Result(result, result.back().time,
                                  result.back().length, baseTime));
    }

    return std::unique_ptr<std::vector<Result>>(results);
}

std::unique_ptr<std::vector<Result>> Routing::Algorithms::AlternativesAlgorithm::GetResults(int startId, int endId,
                                                                                            int startTime) {
    return GetResults(startId, endId, 3, true, startTime);
}

Routing::Algorithms::TravelCostCalculator *Routing::Algorithms::AlternativesAlgorithm::GetTravelCostCalculator() {
    return costCalculator;
}

Routing::Algorithms::TravelTimeCalculator *Routing::Algorithms::AlternativesAlgorithm::GetTravelTimeCalculator() {
    return timeCalculator;
}

Routing::Algorithms::AlternativesAlgorithm::AlternativesAlgorithm(std::shared_ptr<GraphMemory> routingGraph,
                                                                  Routing::Algorithms::AlgorithmSettings settings,
                                                                  TravelCostCalculator *travelCostCalculator,
                                                                  TravelTimeCalculator *travelTimeCalculator)
        : Algorithm(std::move(routingGraph), settings), costCalculator(travelCostCalculator),
          timeCalculator(travelTimeCalculator) {}


