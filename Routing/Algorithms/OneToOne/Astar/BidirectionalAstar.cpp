#include "BidirectionalAstar.h"

std::vector<Segment> Routing::Algorithms::BidirectionalAstar::GetRoute(int startId, int endId, CostCalcType costType) {

    VisitedNodeHashMap closedSetForth;
    closedSetForth.set_empty_key(std::numeric_limits<int>::min());
    BinHeap openSetForth;

    VisitedNodeHashMap closedSetBack;
    closedSetBack.set_empty_key(std::numeric_limits<int>::min());
    BinHeap openSetBack;

    Node startNode, endNode;

    try {
        startNode = this->routingGraph->GetNodeById(startId);
        endNode = this->routingGraph->GetNodeById(endId);
    }
    catch (NodeNotFoundException &e) {
        std::cout << std::string(e.what()) << "(" << e.nodeId << ")" << std::endl;
        return std::vector<Segment>();
    }

    closedSetForth[startNode.id] = VisitedNode(&startNode, 0, true, 0);

    openSetForth.Add(0, startNode.id);

    closedSetBack[endNode.id] = VisitedNode(&endNode, 0, true, 0);

    openSetBack.Add(0, endNode.id);

    int id = -1;
    float minCost = numeric_limits<float>::max();

    while (openSetForth.Count() != 0 || openSetBack.Count() != 0) {
        if (openSetForth.Count() != 0) {
            id = this->AstarForth(endNode, openSetForth, closedSetForth, closedSetBack);

            if (unlikely(id != -1)) {
                auto cost = closedSetForth[id].TotalCost + closedSetBack[id].TotalCost;

                if (cost < minCost) {
                    minCost = cost;
                }

                auto heapCost = openSetForth.PeekKey() + openSetBack.PeekKey();

                if (heapCost >= minCost) break;
            }
        } else break;

        if (openSetBack.Count() != 0) {
            id = this->AstarBack(startNode, openSetBack, closedSetBack, closedSetForth);

            if (unlikely(id != -1)) {
                auto cost = closedSetForth[id].TotalCost + closedSetBack[id].TotalCost;

                if (cost < minCost) {
                    minCost = cost;
                }

                auto heapCost = openSetForth.PeekKey() + openSetBack.PeekKey();

                if (heapCost >= minCost) break;
            }
        } else break;
    }

    if (id != -1) {

#ifndef NDEBUG
        std::cout << "BidirectionalAstar - getting roads: closedSetForth: " << closedSetForth.size() <<
                  ", closedSetBack: " << closedSetBack.size() << std::endl;
#endif

        return AlgorithmHelper::GetRoadsBidirectionalNew(closedSetForth, closedSetBack, startId, endNode.id, id);
    } else {
        return std::vector<Segment>();
    }
}

int Routing::Algorithms::BidirectionalAstar::AstarForth(const Node &endNode, BinHeap &openSetForth,
                                                        VisitedNodeHashMap &closedSetForth,
                                                        VisitedNodeHashMap &closedSetBack) {

    int actualId = openSetForth.Remove();

    auto actualNodeFind = closedSetBack.find(actualId);

    if (actualNodeFind != closedSetBack.end() && actualNodeFind->second.WasUsed) {
        return actualId;
    }

    VisitedNode &tmp = closedSetForth[actualId]; // nepouzivat pres referenci - nefunguje s dense map
    tmp.SetWasUsed(true);
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
        float totalTime = actualNode.CurrentTime + travelTime;
        float distanceToEnd = DistanceCalculator::GetDistance(node2, endNode);
        float totalCost = heuristicCostCalculator->GetTravelCost(edge.length, totalTime, edge.GetFuncClass(),
                                                                 distanceToEnd * this->GetHeuristicFactor());

        auto node2Find = closedSetForth.find(node2.id);

        if (node2Find == closedSetForth.end()) {
            closedSetForth[node2.id] = VisitedNode(&node2, actualId, &edge, totalCost, travelTime, false, totalTime);
            openSetForth.Add(totalCost, node2.id);

        } else if (!node2Find->second.WasUsed) {
            if (node2Find->second.TotalCost > totalCost) {
                openSetForth.UpdatePriority(totalCost, node2.id);
                node2Find->second.Update(actualId, &edge, totalCost, travelTime, totalTime);
            }
        }
    }

    return -1;
}

int Routing::Algorithms::BidirectionalAstar::AstarBack(const Node &endNode,
                                                       BinHeap &openSetForth,
                                                       VisitedNodeHashMap &closedSetForth,
                                                       VisitedNodeHashMap &closedSetBack) {
    int actualId = openSetForth.Remove();

    auto actualNodeFind = closedSetBack.find(actualId);

    if (actualNodeFind != closedSetBack.end() && actualNodeFind->second.WasUsed) {
        return actualId;
    }

    VisitedNode &tmp = closedSetForth[actualId]; // nepouzivat pres referenci - nefunguje s dense map
    tmp.SetWasUsed(true);
    const VisitedNode actualNode = tmp;

    const auto &edges = this->routingGraph->GetEdgesIn(actualId);

    for (unsigned int i = 0; i < edges.size(); ++i) {
        const Edge &edge = *edges[i];
        const Node &node2 = this->routingGraph->GetStartNodeByEdge(edge);

        if (!this->IsAccessible(edge.GetEdgeData())) {
            continue;
        }

        float speedMPS = (this->settings.maxVelocity < edge.GetSpeed() ?
                          this->settings.maxVelocity : edge.GetSpeed()) * Constants::OneDiv3P6;

        float travelTime = timeCalculator->GetTravelTime(edge.length, speedMPS);
        float totalTime = actualNode.CurrentTime + travelTime;
        float distanceToEnd = DistanceCalculator::GetDistance(node2, endNode);
        float totalCost = heuristicCostCalculator->GetTravelCost(edge.length, totalTime, edge.GetFuncClass(),
                                                                 distanceToEnd * this->GetHeuristicFactor());

        auto node2Find = closedSetForth.find(node2.id);

        if (node2Find == closedSetForth.end()) {
            closedSetForth[node2.id] = VisitedNode(&node2, actualId, &edge, totalCost, travelTime, false, totalTime);
            openSetForth.Add(totalCost, node2.id);

        } else if (!node2Find->second.WasUsed) {
            if (node2Find->second.TotalCost > totalCost) {
                openSetForth.UpdatePriority(totalCost, node2.id);
                node2Find->second.Update(actualId, &edge, totalCost, travelTime, totalTime);
            }
        }
    }

    return -1;
}

Routing::Algorithms::BidirectionalAstar::BidirectionalAstar(std::shared_ptr<GraphMemory> routingGraph,
                                                            Routing::Algorithms::AlgorithmSettings settings,
                                                            Routing::Algorithms::HeuristicTravelCostCalculator *travelCostCalculator,
                                                            Routing::Algorithms::TravelTimeCalculator *travelTimeCalculator)
        :
        Routing::Algorithms::GoalDirectedAlgorithm(move(routingGraph), settings, travelCostCalculator,
                                                   travelTimeCalculator) {}





