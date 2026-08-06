#include "AlternativesAlgorithm.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace {
bool NumericDiagnosticsEnabled() {
    static const bool enabled = std::getenv("RUTH_ROUTING_NUMERIC_DIAGNOSTICS") != nullptr;
    return enabled;
}
}

std::unique_ptr<std::vector<Result>> Routing::Algorithms::AlternativesAlgorithm::GetResults(
        int startId, int endId, unsigned int maxRoutes, bool multiThreading, int startTime,
        bool useOriginSpeed) const {

    TimeWatch watch(true);

    auto alternatives = this->GetRoutes(startId, endId, maxRoutes, multiThreading, startTime, useOriginSpeed);

    if (alternatives.empty()) {
        if (NumericDiagnosticsEnabled()) {
            std::fprintf(stderr,
                         "ROUTING_NUMERIC_DIAGNOSTIC stage=no_alternatives start=%d end=%d filtered=%d\n",
                         startId, endId, !settings.filterSettings.allFilterOff);
        }
        return nullptr;
    }

    watch.Stop();

    long baseTime = watch.GetElapsed();

    auto results = std::make_unique<std::vector<Result>>();
    results->reserve(alternatives.size());

    for (auto& result : alternatives) {
        if (!result.empty()) {
            const float finalTime = result.back().time;
            if (std::isfinite(finalTime)) {
                results->emplace_back(result, result.back().time,
                                      result.back().length, baseTime);
            } else if (NumericDiagnosticsEnabled()) {
                std::fprintf(stderr,
                             "ROUTING_NUMERIC_DIAGNOSTIC stage=invalid_final_time start=%d end=%d filtered=%d time=%g length=%d\n",
                             startId, endId, !settings.filterSettings.allFilterOff,
                             static_cast<double>(finalTime), result.back().length);
            }
        } else if (NumericDiagnosticsEnabled()) {
            std::fprintf(stderr,
                         "ROUTING_NUMERIC_DIAGNOSTIC stage=empty_route start=%d end=%d filtered=%d\n",
                         startId, endId, !settings.filterSettings.allFilterOff);
        }
    }

    if(results->empty()){
        return nullptr;
    }

    return results;
}

std::unique_ptr<std::vector<Result>> Routing::Algorithms::AlternativesAlgorithm::GetResults(int startId, int endId,
                                                                                            int startTime) const {
    return GetResults(startId, endId, 3, true, startTime, false);
}

Routing::Algorithms::AlternativesAlgorithm::AlternativesAlgorithm(std::shared_ptr<GraphMemory> routingGraph,
                                                                  const Routing::Algorithms::AlgorithmSettings &settings,
                                                                  const TravelCostCalculator *travelCostCalculator,
                                                                  const TravelTimeCalculator *travelTimeCalculator)
        : Algorithm(std::move(routingGraph), settings), costCalculator(travelCostCalculator),
          timeCalculator(travelTimeCalculator) {}

