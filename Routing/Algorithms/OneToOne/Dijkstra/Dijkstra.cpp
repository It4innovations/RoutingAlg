//
// Created by jsevcik on 3/3/16.
//

#include "Dijkstra.h"

std::vector<Segment> Routing::Algorithms::Dijkstra::GetRoute(int startId, int endId, CostCalcType costType) {

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

    closedSet[startNode.id] = VisitedNode(&startNode, 0, true);
    openSet.Add(0, startNode.id);

    while (openSet.Count() != 0) {
        int actualId = openSet.Remove();

        if (unlikely(endNode.id == actualId)) {
            break;
        }

        VisitedNode &tmp = closedSet[actualId]; // nepouzivat pres referenci - nefunguje s dense map
        tmp.SetWasUsed(true);

        //#REFACTORING# -- useless?? but without this crushing
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

            float travelTime = timeCalculator->GetTravelTime(edge.length, speedMPS);
            float totalCost =
                    actualNode.TotalCost + costCalculator->GetTravelCost(edge.length, travelTime, edge.GetFuncClass());

            auto node2Find = closedSet.find(node2.id);

            if (node2Find == closedSet.end()) {
                closedSet[node2.id] = VisitedNode(&node2, actualId, &edge, totalCost, travelTime, false);
                openSet.Add(totalCost, node2.id);
            } else {
                if (!node2Find->second.WasUsed) {
                    if (node2Find->second.TotalCost > totalCost) {
                        openSet.UpdatePriority(totalCost, node2.id);
                        node2Find->second.Update(actualId, &edge, totalCost, travelTime);
                    }
                }
            }
        }
    }

    auto result = AlgorithmHelper::GetRoadsNew(closedSet, startNode.id, endNode.id);

#ifndef NDEBUG
    std::cout << "Dijkstra - getting roads: closedSet: " << closedSet.size() << ", endNodeId: " << endNode.id
              << std::endl;
#endif
    return result;
}

Routing::Algorithms::Dijkstra::Dijkstra(std::shared_ptr<GraphMemory> routingGraph,
                                        Routing::Algorithms::AlgorithmSettings settings,
                                        TravelCostCalculator *travelCostCalculator,
                                        TravelTimeCalculator *travelTimeCalculator) :
        Routing::Algorithms::StaticRoutingAlgorithm(move(routingGraph), settings, travelCostCalculator,
                                                    travelTimeCalculator) {}
