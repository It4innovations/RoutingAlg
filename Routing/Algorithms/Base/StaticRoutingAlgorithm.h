#pragma once

#include <memory>

#include "Algorithm.h"
#include "../Common/Result.h"
#include "../Common/AlgorithmHelper.h"
#include "../TravelCostCalculators/TravelCostCalculator.h"
#include "../TravelCostCalculators/TravelTimeCalculator.h"
#include "../TravelCostCalculators/CostCalculator.h"
#include "../../Data/Utility/TimeWatch.h"
#include "../../Data/Utility/Constants.h"
#include "../../Data/Utility/GPSDistanceCalculation.h"
#include "../GraphFilter/GraphFilterGeometry.h"
#include "../GraphFilter/GraphFilterGeometryStartLocation.h"
#include "../GraphFilter/GraphFilterBase.h"
#include "../GraphFilter/GraphFilterHierarchy.h"

using Routing::Data::GraphMemory;
using Routing::Algorithms::Segment;
using Routing::Utils::TimeWatch;

namespace Routing {

    namespace Algorithms {

        class StaticRoutingAlgorithm : public Algorithm {
        public:
            StaticRoutingAlgorithm(std::shared_ptr<GraphMemory> routingGraph,
                                   AlgorithmSettings settings = AlgorithmSettings(),
                                   TravelCostCalculator *travelCostCalculator = &TravelCostCalculator::Instance(),
                                   TravelTimeCalculator *travelTimeCalculator = &TravelTimeCalculator::Instance());

            virtual ~StaticRoutingAlgorithm() {};

            std::unique_ptr<Result> GetResult(int startId, int endId, CostCalcType costType);

            void GetStartInfo(Node startNode, Node endNode, bool &longDistance);

            TravelCostCalculator *GetTravelCostCalculator();

            TravelTimeCalculator *GetTravelTimeCalculator();

        protected:
            TravelCostCalculator *costCalculator;
            TravelTimeCalculator *timeCalculator;

            virtual std::vector<Segment> GetRoute(int startId, int endId, CostCalcType costType) = 0;

            GraphFilterGeometry
            InitiateFilters(Node startNode, Node endNode, Node startPositionNode, bool longDistance);
        };
    }
}
