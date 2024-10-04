#pragma once

#include <sparsehash/dense_hash_map>
#include <sparsehash/dense_hash_set>

#include "../TravelCostCalculators/CostCalculator.h"
#include "../TravelCostCalculators/TravelCostCalculator.h"
#include "../TravelCostCalculators/TravelTimeCalculator.h"
#include "../Common/AlgorithmHelper.h"
#include "../Common/AlgorithmSettings.h"
#include "../Common/DistanceResult.h"
#include "../Common/DistanceInfo.h"
#include "../Common/VisitedNode.h"
#include "../Base/Algorithm.h"
#include "../GraphFilter/GraphFilterGeometry.h"
#include "../../Data/Collections/BinaryHeap.h"
#include "../../Data/Indexes/GraphMemory.h"
#include "../../Data/Utility/Constants.h"
#include "../../Data/Utility/TimeWatch.h"
#include "../../Data/Utility/Definitions.h"



using Routing::Algorithms::AlgorithmHelper;
using Routing::Data::GraphMemory;

namespace Routing {

    namespace Algorithms {

        class OneToManyDijkstra : public Algorithm {
        public:
            OneToManyDijkstra(std::shared_ptr<Routing::Data::GraphMemory> routingGraph,
                              AlgorithmSettings settings = AlgorithmSettings(),
                              TravelCostCalculator *travelCostCalculator = &TravelCostCalculator::Instance(),
                              TravelTimeCalculator *travelTimeCalculator = &TravelTimeCalculator::Instance());

            std::unique_ptr<Routing::Algorithms::DistanceResult>
            GetDistances(int startId, const std::vector<int> &endIds, int endId = -1);

            TravelCostCalculator *GetTravelCostCalculator();

            TravelTimeCalculator *GetTravelTimeCalculator();

        protected:
            TravelCostCalculator *costCalculator;
            TravelTimeCalculator *timeCalculator;

            std::vector<DistanceInfo> GetDistanceInfos(
                    int startId, const std::vector<int> &endIds, int endId);

            Routing::Algorithms::GraphFilterGeometry InitiateCircleFilter(Node &startNode, int endId,
                                                                          const std::vector<int> &endIds);
        };
    }
}



