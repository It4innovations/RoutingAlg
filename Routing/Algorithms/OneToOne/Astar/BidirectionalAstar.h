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

typedef google::dense_hash_map<int, Routing::Algorithms::VisitedNode> VisitedNodeHashMap;
typedef Routing::Collections::BinaryHeap<float, int> BinHeap;

namespace Routing {

    namespace Algorithms {

        class BidirectionalAstar : public GoalDirectedAlgorithm {
        public:
            BidirectionalAstar(std::shared_ptr<GraphMemory> routingGraph,
                               AlgorithmSettings settings = AlgorithmSettings(),
                               HeuristicTravelCostCalculator *travelCostCalculator = &HeuristicTravelCostCalculator::Instance(),
                               TravelTimeCalculator *travelTimeCalculator =
                               &TravelTimeCalculator::Instance());

        private:
            int AstarForth(const Node &endNode, BinHeap &openSetForth, VisitedNodeHashMap &closedSetForth,
                           VisitedNodeHashMap &closedSetBack);

            int AstarBack(const Node &endNode, BinHeap &openSetForth, VisitedNodeHashMap &closedSetForth,
                          VisitedNodeHashMap &closedSetFortha);

        protected:
            std::vector<Segment> GetRoute(int startId, int endId, CostCalcType costType) override final;
        };
    }
}


