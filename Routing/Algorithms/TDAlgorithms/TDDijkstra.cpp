
#include "TDDijkstra.h"

std::vector<Routing::Algorithms::Segment> Routing::Algorithms::TD::TDDijkstra::GetRoute(
        int startId, int endId, int startTime) {

    VisitedNodeHashMap closedSet;
    closedSet.set_empty_key(std::numeric_limits<int>::min());
    BinHeap openSet;

    Node startNode;

    try {
        startNode = this->routingGraph->GetNodeById(startId);
    }
    catch (Routing::Exception::NodeNotFoundException &e) {
        std::cout << std::string(e.what()) << "(" << e.nodeId << ")" << std::endl;
        return std::vector<Segment>();
    }

    closedSet[startNode.id] = VisitedNode(&startNode, startTime, true);
    openSet.Add(0, startNode.id);

    auto filterRoot = InitiateFilters(Routing::Algorithms::GraphFilterGeometry::Ellipse,
                                      routingGraph->GetNodeById(startId), routingGraph->GetNodeById(endId));

    FindPath(endId, closedSet, openSet, filterRoot);

    filterRoot.DeleteAllFilter();

    auto result = AlgorithmHelper::GetRoadsNew(closedSet, startId, endId);

    std::cout << "Time of route: " << result.back().time << " sec" << std::endl;

    return result;
}

void Routing::Algorithms::TD::TDDijkstra::FindPath(int endId, VisitedNodeHashMap &closedSet, BinHeap &openSet,
                                                   Routing::Algorithms::GraphFilterGeometry &filterRoot) {

    while (openSet.Count() != 0) {
        int actualId = openSet.Remove();

        if (unlikely(endId == actualId)) {
            break;
        }

        VisitedNode &tmp = closedSet[actualId]; // nepouzivat pres referenci - nefunguje s dense map
        tmp.SetWasUsed(true);

        const VisitedNode actualNode = tmp;

        const auto &edges = this->routingGraph->GetEdgesOut(actualId);

        for (unsigned int i = 0; i < edges.size(); ++i) {
            const Edge &edge = *edges[i];
            Node node2 = this->routingGraph->GetEndNodeByEdge(edge);

            if (!this->IsAccessible(edge.GetEdgeData())) {
                continue;
            }

            bool filtersPass = true;

            filterRoot.SetAllFiltersData(node2, *edges[i]);
            filterRoot.Handle(filtersPass);

            if (!filtersPass) {
                continue;
            }

            float travelTime;

            try {
                auto profile = this->storage->GetSpeedProfileByEdgeId(edge.edgeId);
                travelTime = Routing::Algorithms::TD::CalculateCostSecPrecise(edge, actualNode.TotalCost, profile,
                                                                              this->storage);
            }
            catch (Routing::Exception::ProfileNotFound& e) {
                travelTime = Routing::Algorithms::TD::CalculateCostSec(edge, edge.GetSpeed());
            }

            float totalCost = actualNode.TotalCost + travelTime;

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
}

Routing::Algorithms::GraphFilterGeometry Routing::Algorithms::TD::TDDijkstra::InitiateFilters(
        GraphFilterGeometry::FilterType typeOfFilter, Node startNode, Node endNode) {

    GraphFilterGeometry filterGeometryRoot(typeOfFilter, startNode, endNode);

    return filterGeometryRoot;
}

std::unique_ptr<Routing::Algorithms::Result> Routing::Algorithms::TD::TDDijkstra::GetResult(
        int startId, int endId, int startTime) {

    TimeWatch watch(true);

    auto route = this->GetRoute(startId, endId, startTime);

    if (route.empty()) {
        return nullptr;
    }

    watch.Stop();

    long baseTime = watch.GetElapsed();

    return std::unique_ptr<Result>(new Result(route, route.back().time, route.back().length, baseTime));
}