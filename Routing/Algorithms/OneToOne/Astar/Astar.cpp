
#include "Astar.h"

std::vector<Segment> Routing::Algorithms::Astar::GetRoute(int startId, int endId, CostCalcType costType) {

    VisitedNodeHashMap closedSet;
    closedSet.set_empty_key(std::numeric_limits<int>::min());
    BinHeap openSet;

    Node startNode, endNode;

    try {
        startNode = this->routingGraph->GetNodeById(startId);
        endNode = this->routingGraph->GetNodeById(endId);
    }
    catch (NodeNotFoundException &e) {
        std::cout << std::string(e.what()) << "(" << e.nodeId << ")" << std::endl;
        return std::vector<Segment>();
    }

    closedSet[startNode.id] = VisitedNode(&startNode, 0, true, 0);
    openSet.Add(0, startNode.id);

    while (openSet.Count() != 0) {
        int actualId = openSet.Remove();

        if (endNode.id == actualId) {
            break;
        }

        VisitedNode &tmp = closedSet[actualId]; // nepouzivat pres referenci - nefunguje s dense map
        tmp.WasUsed = true;
        const VisitedNode actualNode = tmp;

        const auto &edges = this->routingGraph->GetEdgesOut(actualId);

        for (unsigned int i = 0; i < edges.size(); ++i) {
            const Edge &edge = *edges[i];
            const Node &node2 = this->routingGraph->GetEndNodeByEdge(edge);

            if (!this->IsAccessible(edge.GetEdgeData())) {
                continue;
            }

            float speedMPS = (this->settings.maxVelocity < edge.GetSpeed() ?
                              this->settings.maxVelocity : edge.GetSpeed()) * Constants::OneDiv3P6;

            //TODO calculators
            float travelTime = timeCalculator->GetTravelTime(edge.length, speedMPS);
            float totalTime = actualNode.CurrentTime + travelTime;
            float distanceToEnd = DistanceCalculator::GetDistance(node2, endNode);
            float totalCost = heuristicCostCalculator->GetTravelCost(edge.length, totalTime, edge.GetFuncClass(),
                                                                     distanceToEnd * this->GetHeuristicFactor());

            auto node2Find = closedSet.find(node2.id);

            if (node2Find == closedSet.end()) {
                closedSet[node2.id] = VisitedNode(&node2, actualId, &edge, totalCost, travelTime, false, totalTime);
                openSet.Add(totalCost, node2.id);
            } else if (!node2Find->second.WasUsed) {
                if (node2Find->second.TotalCost > totalCost) {
                    openSet.UpdatePriority(totalCost, node2.id);
                    node2Find->second.Update(actualId, &edge, totalCost, travelTime, totalTime);
                }
            }
        }
    }

    auto result = AlgorithmHelper::GetRoadsNew(closedSet, startNode.id, endNode.id);

#ifndef NDEBUG
    std::cout << "Astar - getting roads: closedSet: " << closedSet.size() << ", endNodeId: " << endNode.id << std::endl;
#endif

    return result;
}

Routing::Algorithms::Astar::Astar(std::shared_ptr<GraphMemory> routingGraph,
                                  Routing::Algorithms::AlgorithmSettings settings,
                                  Routing::Algorithms::HeuristicTravelCostCalculator *travelCostCalculator,
                                  Routing::Algorithms::TravelTimeCalculator *travelTimeCalculator)
        :
        Routing::Algorithms::GoalDirectedAlgorithm(routingGraph, settings, travelCostCalculator,
                                                   travelTimeCalculator) {}