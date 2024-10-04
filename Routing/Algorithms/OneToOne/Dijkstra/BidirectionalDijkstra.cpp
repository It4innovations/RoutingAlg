//
// Created by jsevcik on 3/31/16.
//

#include "BidirectionalDijkstra.h"

std::vector<Segment> Routing::Algorithms::BidirectionalDijkstra::GetRoute(int startId, int endId,
                                                                          CostCalcType costType) {

    google::dense_hash_map<int, VisitedNode> closedSetForth;
    closedSetForth.set_empty_key(std::numeric_limits<int>::min());
    BinHeap openSetForth;

    google::dense_hash_map<int, VisitedNode> closedSetBack;
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

    closedSetForth[startNode.id] = VisitedNode(&startNode, 0, true);

    openSetForth.Add(0, startNode.id);

    closedSetBack[endNode.id] = VisitedNode(&endNode, 0, true);

    openSetBack.Add(0, endNode.id);

    bool longDistance = false;

    GetStartInfo(startNode, endNode, longDistance);

    auto filterGeometryRootForth = InitiateFilters(startNode, endNode, startNode, longDistance);
    auto filterGeometryRootBack = InitiateFilters(startNode, endNode, endNode, longDistance);

    int id = FindPath(openSetForth, closedSetForth, openSetBack, closedSetBack, costType, filterGeometryRootForth,
                      filterGeometryRootBack, false);

    filterGeometryRootForth.DeleteAllFilter();
    filterGeometryRootBack.DeleteAllFilter();

    if (id != -1) {

#ifndef NDEBUG
        std::cout << "BidirectionalDijkstra - getting roads: closedSetForth: " << closedSetForth.size() <<
                  ", closedSetBack: " << closedSetBack.size() << std::endl;
#endif

        return AlgorithmHelper::GetRoadsBidirectionalNew(closedSetForth, closedSetBack, startNode.id, endNode.id, id);

    }


    return std::vector<Segment>();
}


int Routing::Algorithms::BidirectionalDijkstra::FindPath(BinHeap &openSetForth,
                                                         google::dense_hash_map<int, Algorithms::VisitedNode> &closedSetForth,
                                                         BinHeap &openSetBack,
                                                         google::dense_hash_map<int, Algorithms::VisitedNode> &closedSetBack,
                                                         CostCalcType costType, GraphFilterGeometry plateauFilterForth,
                                                         GraphFilterGeometry plateauFilterBack, bool allFilterOff) {

    int id = -1;
    float minCost = numeric_limits<float>::max();

    const Routing::CostCalculator calc(costType, this->settings.maxVelocity);

    while (openSetForth.Count() != 0 || openSetBack.Count() != 0) {
        if (openSetForth.Count() != 0) {
            id = this->DijkstraForth(openSetForth, closedSetForth, closedSetBack, calc, plateauFilterForth,
                                     allFilterOff);

            if (unlikely(id != -1)) {
                float cost = closedSetForth[id].TotalCost + closedSetBack[id].TotalCost;

                if (cost < minCost) {
                    minCost = cost;
                }

                float heapCost = openSetForth.PeekKey() + openSetBack.PeekKey();

                if (heapCost >= minCost) {
                    break;
                }
            }
        } else {
            break;
        }

        if (openSetBack.Count() != 0) {
            id = this->DijkstraBack(openSetBack, closedSetBack, closedSetForth, calc, plateauFilterBack, allFilterOff);

            if (unlikely(id != -1)) {
                float cost = closedSetForth[id].TotalCost + closedSetBack[id].TotalCost;

                if (cost < minCost) {
                    minCost = cost;
                }

                float heapCost = openSetForth.PeekKey() + openSetBack.PeekKey();

                if (heapCost >= minCost) {
                    break;
                }
            }
        } else {
            break;
        }
    }

    return id;
}

int Routing::Algorithms::BidirectionalDijkstra::DijkstraForth(BinHeap &openSetForth, VisitedNodeHashMap &closedSetForth,
                                                              VisitedNodeHashMap &closedSetBack,
                                                              const CostCalculator &calc,
                                                              GraphFilterGeometry plateauFilter, bool allFilterOff) {

    int actualId = openSetForth.Remove();

    auto actualNodeFind = closedSetBack.find(actualId);

    if (actualNodeFind != closedSetBack.end() && actualNodeFind->second.WasUsed) {
        return actualId;
    }

    VisitedNode &tmp = closedSetForth[actualId];
    tmp.SetWasUsed(true);
    VisitedNode actualNode = tmp;

    const auto &edges = this->routingGraph->GetEdgesOut(actualId);

    for (unsigned int i = 0; i < edges.size(); ++i) {
        Edge &edge = *edges[i];
        const Node &node2 = this->routingGraph->GetEndNodeByEdge(edge);

        if (!this->IsAccessible(edge.GetEdgeData())) {
            continue;
        }

        bool filtersPass = true;

        if (!allFilterOff) {
            plateauFilter.SetAllFiltersData(node2, edge);
            plateauFilter.Handle(filtersPass);
        }

        if (!filtersPass) {
            continue;
        }

        float speedMPS = (this->settings.maxVelocity < edge.GetSpeed() ?
                          this->settings.maxVelocity : edge.GetSpeed()) * Constants::OneDiv3P6;

        float travelTime = timeCalculator->GetTravelTime(edge.length, speedMPS);
        float totalCost =
                actualNode.TotalCost + costCalculator->GetTravelCost(edge.length, travelTime, edge.GetFuncClass());

        auto node2Find = closedSetForth.find(node2.id);

        if (node2Find == closedSetForth.end()) {
            closedSetForth[node2.id] = VisitedNode(&node2, actualId, &edge,
                                                   totalCost, travelTime, false);
            openSetForth.Add(totalCost, node2.id);

        } else if (!node2Find->second.WasUsed) {
            if (node2Find->second.TotalCost > totalCost) {
                openSetForth.UpdatePriority(totalCost, node2.id);
                node2Find->second.Update(actualId, &edge, totalCost, travelTime);
            }
        }
    }

    return -1;
}

int Routing::Algorithms::BidirectionalDijkstra::DijkstraBack(BinHeap &openSetBack, VisitedNodeHashMap &closedSetBack,
                                                             VisitedNodeHashMap &closedSetForth,
                                                             const CostCalculator &calc,
                                                             GraphFilterGeometry plateauFilter, bool allFilterOff) {

    int actualId = openSetBack.Remove();

    auto actualNodeFind = closedSetForth.find(actualId);

    if (actualNodeFind != closedSetForth.end() && actualNodeFind->second.WasUsed) {
        return actualId;
    }

    VisitedNode &tmp = closedSetBack[actualId];
    tmp.SetWasUsed(true);
    VisitedNode actualNode = tmp;

    const auto &edges = this->routingGraph->GetEdgesIn(actualId);

    for (unsigned int i = 0; i < edges.size(); ++i) {
        Edge &edge = *edges[i];
        const Node &node2 = this->routingGraph->GetStartNodeByEdge(edge);

        if (!this->IsAccessible(edge.GetEdgeData())) {
            continue;
        }

        bool filtersPass = true;

        if (!allFilterOff) {
            plateauFilter.SetAllFiltersData(node2, edge);
            plateauFilter.Handle(filtersPass);
        }

        if (!filtersPass) {
            continue;
        }

        float speedMPS = (this->settings.maxVelocity < edge.GetSpeed() ?
                          this->settings.maxVelocity : edge.GetSpeed()) * Constants::OneDiv3P6;

        float travelTime = timeCalculator->GetTravelTime(edge.length, speedMPS);
        float totalCost =
                actualNode.TotalCost + costCalculator->GetTravelCost(edge.length, travelTime, edge.GetFuncClass());

        auto node2Find = closedSetBack.find(node2.id);

        if (node2Find == closedSetBack.end()) {
            closedSetBack[node2.id] = VisitedNode(&node2, actualId, &edge, totalCost, travelTime, false);
            openSetBack.Add(totalCost, node2.id);

        } else if (!node2Find->second.WasUsed) {
            if (node2Find->second.TotalCost > totalCost) {
                openSetBack.UpdatePriority(totalCost, node2.id);
                node2Find->second.Update(actualId, &edge, totalCost, travelTime);
            }
        }
    }

    return -1;
}

Routing::Algorithms::BidirectionalDijkstra::BidirectionalDijkstra(std::shared_ptr<GraphMemory> routingGraph,
                                                                  Routing::Algorithms::AlgorithmSettings settings,
                                                                  TravelCostCalculator *travelCostCalculator,
                                                                  TravelTimeCalculator *travelTimeCalculator)
        :
        Routing::Algorithms::StaticRoutingAlgorithm(move(routingGraph), settings, travelCostCalculator,
                                                    travelTimeCalculator) {}




