#pragma once

#include <ostream>
#include "../../Data/Structures/Node.h"
#include "../../Data/Structures/Edge.h"

namespace Routing {

    namespace Algorithms {

        class VisitedNode {

        public:
            const Node *NodeP;
            int PreviousNodeId;
            const Edge *EdgeP;
            float TotalCost;
            float TravelTime;
            bool WasUsed;
            float CurrentTime;

            VisitedNode();

            VisitedNode(const Node *node, float totalCost, bool wasUsed, float currentTime = -1);

            VisitedNode(const Node *node, int previousNodeId, const Edge *edge, float totalCost, float travelTime,
                        bool wasUsed, float currentTime = -1);

            inline void SetWasUsed(bool wasUsed) {
                this->WasUsed = wasUsed;
            }

            void
            Update(int previousNodeId, const Edge *edge, float totalCost, float travelTime, float currentTime = -1);

            friend std::ostream &operator<<(std::ostream &os, const VisitedNode &visitedNode) {
                os << visitedNode.NodeP->id << ", " <<
                   visitedNode.PreviousNodeId << ", " <<
                   visitedNode.EdgeP->edgeId << ", " <<
                   visitedNode.TotalCost << ", " <<
                   visitedNode.TravelTime << ", " <<
                   visitedNode.EdgeP->length << ", " <<
                   visitedNode.WasUsed << ", " <<
                   visitedNode.CurrentTime;

                return os;
            }
        };
    }
}
