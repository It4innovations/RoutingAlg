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
#include "../GraphFilter/GraphFilterGeometry.h"

using Routing::Data::GraphMemory;
using Routing::Algorithms::Segment;
using Routing::Algorithms::Result;
using Routing::Utils::TimeWatch;

namespace Routing {

    namespace Algorithms {

        class AlternativesAlgorithm : public Algorithm {
        public:
            explicit AlternativesAlgorithm(std::shared_ptr<GraphMemory> routingGraph,
                                  AlgorithmSettings settings = AlgorithmSettings(),
                                  TravelCostCalculator *travelCostCalculator = &TravelCostCalculator::Instance(),
                                  TravelTimeCalculator *travelTimeCalculator = &TravelTimeCalculator::Instance());

            std::unique_ptr<std::vector<Result>> GetResults(int startId, int endId, unsigned int maxRoutes,
                                                            bool multiThreading, int startTime = 0);

            std::unique_ptr<std::vector<Result>> GetResults(int startId, int endId, int startTime = 0);

            TravelCostCalculator *GetTravelCostCalculator();

            TravelTimeCalculator *GetTravelTimeCalculator();

        protected:
            TravelCostCalculator *costCalculator;
            TravelTimeCalculator *timeCalculator;

            virtual std::vector<std::vector<Segment>> GetRoutes(
                    int startId, int endId, unsigned int maxRoutes, bool multiThreading,
                    int startTime) = 0;
        };
    }
}

