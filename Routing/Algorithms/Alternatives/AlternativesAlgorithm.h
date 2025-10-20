#pragma once

#include <memory>

#include "../Common/AlgorithmSettings.h"
#include "../Common/Result.h"
#include "../Common/AlgorithmHelper.h"
#include "../TravelCostCalculators/TravelCostCalculator.h"
#include "../TravelCostCalculators/TravelTimeCalculator.h"
#include "../../Data/Utility/TimeWatch.h"
#include "../Base/Algorithm.h"
#include "../../Data/Indexes/GraphMemory.h"

using Routing::Data::GraphMemory;
using Routing::Algorithms::Segment;
using Routing::Algorithms::Result;
using Routing::Utils::TimeWatch;

namespace Routing::Algorithms {

    class AlternativesAlgorithm : public Algorithm {
    public:
        explicit AlternativesAlgorithm(std::shared_ptr<GraphMemory> routingGraph,
                              const AlgorithmSettings &settings = AlgorithmSettings(),
                              const TravelCostCalculator *travelCostCalculator = &TravelCostCalculator::Instance(),
                              const TravelTimeCalculator *travelTimeCalculator = &TravelTimeCalculator::Instance());

        [[nodiscard]] std::unique_ptr<std::vector<Result>> GetResults(int startId, int endId, unsigned int maxRoutes,
                                                        bool multiThreading, int startTime = 0) const;

        [[nodiscard]] std::unique_ptr<std::vector<Result>> GetResults(int startId, int endId, int startTime = 0) const;

    protected:
        const TravelCostCalculator *costCalculator;
        const TravelTimeCalculator *timeCalculator;

        virtual std::vector<std::vector<Segment>> GetRoutes(
                int startId, int endId, unsigned int maxRoutes, bool multiThreading,
                int startTime) const = 0;
    };
}

