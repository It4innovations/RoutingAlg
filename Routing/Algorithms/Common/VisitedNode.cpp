
#include "VisitedNode.h"

Routing::Algorithms::VisitedNode::VisitedNode()
        : NodeP(nullptr),
          PreviousNodeId(-1),
          EdgeP(nullptr),
          TotalCost(-1),
          TravelTime(0),
          WasUsed(false),
          CurrentTime(0) {}

Routing::Algorithms::VisitedNode::VisitedNode(const Node *node, float totalCost, bool wasUsed, float currentTime)
        : NodeP(node),
          PreviousNodeId(-1),
          EdgeP(nullptr),
          TotalCost(totalCost),
          TravelTime(0),
          WasUsed(wasUsed),
          CurrentTime(currentTime) {}

Routing::Algorithms::VisitedNode::VisitedNode(const Node *node, int previousNodeId, const Edge *edge, float totalCost,
                                              float travelTime, bool wasUsed, float currentTime)
        : NodeP(node),
          PreviousNodeId(previousNodeId),
          EdgeP(edge),
          TotalCost(totalCost),
          TravelTime(travelTime),
          WasUsed(wasUsed),
          CurrentTime(currentTime) {}

void Routing::Algorithms::VisitedNode::Update(int previousNodeId, const Edge *edge, float totalCost, float travelTime,
                                              float currentTime) {
    this->PreviousNodeId = previousNodeId;
    this->EdgeP = edge;
    this->TotalCost = totalCost;
    this->TravelTime = travelTime;
    this->CurrentTime = currentTime;
}

