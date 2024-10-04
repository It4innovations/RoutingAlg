#pragma once

#include "../../Base/StaticRoutingAlgorithm.h"
#include "../../Common/AlgorithmHelper.h"
#include "../../../Data/Collections/BinaryHeap.h"
#include <sparsehash/dense_hash_map>
#include "../../Common/VisitedNode.h"
#include "../../../Data/Utility/Definitions.h"

namespace Routing {

    namespace Algorithms {

        class BidirectionalDijkstra : public StaticRoutingAlgorithm {

        public:
            BidirectionalDijkstra(std::shared_ptr<GraphMemory> routingGraph,
                                  AlgorithmSettings settings = AlgorithmSettings(),
                                  TravelCostCalculator *travelCostCalculator = &TravelCostCalculator::Instance(),
                                  TravelTimeCalculator *travelTimeCalculator = &TravelTimeCalculator::Instance());

        private:
            int
            DijkstraForth(BinHeap &openSetForth, VisitedNodeHashMap &closedSetForth, VisitedNodeHashMap &closedSetBack,
                          const CostCalculator &calc, GraphFilterGeometry plateauFilter, bool allFilterOff);

            int
            DijkstraBack(BinHeap &openSetBack, VisitedNodeHashMap &closedSetBack, VisitedNodeHashMap &closedSetForth,
                         const CostCalculator &calc, GraphFilterGeometry plateauFilter, bool allFilterOff);

            int FindPath(BinHeap &openSetForth, VisitedNodeHashMap &closedSetForth, BinHeap &openSetBack,
                         VisitedNodeHashMap &closedSetBack, CostCalcType costType,
                         GraphFilterGeometry plateauFilterForth,
                         GraphFilterGeometry plateauFilterBack, bool allFilterOff);


        protected:
            std::vector<Segment> GetRoute(int startId, int endId, CostCalcType costType) override final;
        };
    }
}


