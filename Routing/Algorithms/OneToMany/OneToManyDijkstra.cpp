
#include "OneToManyDijkstra.h"


std::vector<Routing::Algorithms::DistanceInfo> Routing::Algorithms::OneToManyDijkstra::GetDistanceInfos(
        int startId, const std::vector<int> &endIds, int endId) {

    VisitedNodeHashMap closedSet;
    closedSet.set_empty_key(std::numeric_limits<int>::min());

    BinHeap openSet;

    HashSet endIdsSet;
    endIdsSet.set_empty_key(std::numeric_limits<int>::min());

    HashSet foundEndIds;
    foundEndIds.set_empty_key(std::numeric_limits<int>::min());

    Node startNode;

    try {
        startNode = this->routingGraph->GetNodeById(startId);
        closedSet[startNode.id] = VisitedNode(&startNode, 0, true);
        openSet.Add(0, startNode.id);
    }
    catch (NodeNotFoundException &e) {
        std::cout << std::string(e.what()) << "(" << e.nodeId << ")" << std::endl;
        return std::vector<Routing::Algorithms::DistanceInfo>();
    }

    for (int const &eid : endIds) {
        try {
            Node node = this->routingGraph->GetNodeById(eid);
        }
        catch (NodeNotFoundException &e) {
            std::cout << std::string(e.what()) << "(" << e.nodeId << ")" << std::endl;
            return std::vector<Routing::Algorithms::DistanceInfo>();
        }

        endIdsSet.insert(eid);
    }

    if (endId != -1) {
        try {
            Node node = this->routingGraph->GetNodeById(endId);
        }
        catch (NodeNotFoundException &e) {
            std::cout << std::string(e.what()) << "(" << e.nodeId << ")" << std::endl;
            return std::vector<Routing::Algorithms::DistanceInfo>();
        }
        endIdsSet.insert(endId);
    }

    while (openSet.Count() != 0) {
        int actualId = openSet.Remove();

        auto nodeFind = endIdsSet.find(actualId);

        if (nodeFind != endIdsSet.end()) {
            foundEndIds.insert(actualId);

            if (foundEndIds.size() == endIdsSet.size()) {
                break;
            }

            if (actualId == endId) break;
        }

        VisitedNode &tmp = closedSet[actualId]; // nepouzivat pres referenci - nefunguje s dense map
        tmp.SetWasUsed(true);
        const VisitedNode actualNode = tmp;

        Node node;

        try {
            node = this->routingGraph->GetNodeById(actualId);
        }
        catch (NodeNotFoundException &e) {
            std::cout << std::string(e.what()) << "(" << e.nodeId << ")" << std::endl;
            return std::vector<Routing::Algorithms::DistanceInfo>();
        }

        bool allFilterOff = settings.filterSettings.allFilterOff;

        auto filter = InitiateCircleFilter(startNode, endId, endIds);

        const auto &edges = this->routingGraph->GetEdgesOut(node);

        for (unsigned int i = 0; i < edges.size(); ++i) {
            Edge &edge = *edges[i];
            Node node2 = this->routingGraph->GetEndNodeByEdge(edge);

            if (!this->IsAccessible(edge.GetEdgeData())) {
                continue;
            }

            bool filtersPass = true;

            if(!allFilterOff) {
                filter.SetAllFiltersData(node2, edge);
                filter.Handle(filtersPass);
            }

            if(!filtersPass){
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
            } else if (!node2Find->second.WasUsed) {
                if (node2Find->second.TotalCost > totalCost) {
                    openSet.UpdatePriority(totalCost, node2.id);

                    node2Find->second.Update(actualId, &edge, totalCost, travelTime);
                }
            }
        }
    }

#ifndef NDEBUG
    std::cout << "One to many distance Dijkstra - getting roads: closedSet: " << closedSet.size() << ", endIds: " <<
              endIds.size() << ", found endIds: " << foundEndIds.size() << std::endl;
#endif

    return AlgorithmHelper::GetDistanceInfos(closedSet, startId, foundEndIds);
}

std::unique_ptr<Routing::Algorithms::DistanceResult> Routing::Algorithms::OneToManyDijkstra::GetDistances(
        int startId, const std::vector<int> &endIds, int endId) {

    TimeWatch watch(true);

    auto distances = this->GetDistanceInfos(startId, endIds, endId);

    watch.Stop();

    if (distances.size() == 0) {
        return nullptr;
    }

    long baseTime = watch.GetElapsed();

    return std::unique_ptr<Routing::Algorithms::DistanceResult>(new DistanceResult(distances, baseTime));
}

Routing::Algorithms::GraphFilterGeometry Routing::Algorithms::OneToManyDijkstra::InitiateCircleFilter(
        Node &startNode, int endId, const std::vector<int> &endIds){

    if(!settings.filterSettings.allFilterOff && settings.filterSettings.typeOfFilter != GraphFilterGeometry::FilterType::NotSet) {
        GPSDistanceCalculation::GPSDistanceCalculation distanceCalc;
        double maxRange = -1;

        Node endNode;
        if(endId != -1){
            endNode = this->routingGraph->GetNodeById(endId);
        } else {
            int index = 0;
            int indexMaxRange = 0;

            for(auto &id : endIds){
                Node node = this->routingGraph->GetNodeById(id);
                auto distance = distanceCalc.DistanceEarth((double)startNode.GetLatitude(), (double)startNode.GetLongitude(),
                                                           (double)node.GetLatitude(), (double)node.GetLongitude());
                if(distance > maxRange){
                    indexMaxRange = index;
                    maxRange = distance;
                }

                index++;
            }

            endNode = this->routingGraph->GetNodeById(endIds[indexMaxRange]);
        }

        GraphFilterGeometry filterGeometryRoot(GraphFilterGeometry::FilterType::Circle, startNode, endNode);

        if(endId == -1){
            filterGeometryRoot.SetCenterCircle((double) startNode.GetLongitude(), (double) startNode.GetLatitude(), 2);
        }

        return filterGeometryRoot;
    }

    Node endNode;

    if(endId != -1) {
        endNode = this->routingGraph->GetNodeById(endId);
    } else {
        endNode = this->routingGraph->GetNodeById(endIds[0]);
    }

    GraphFilterGeometry filterGeometryRoot(GraphFilterGeometry::FilterType::NotSet, startNode, endNode);
    return filterGeometryRoot;
}

Routing::Algorithms::TravelCostCalculator *Routing::Algorithms::OneToManyDijkstra::GetTravelCostCalculator() {
    return costCalculator;
}

Routing::Algorithms::TravelTimeCalculator *Routing::Algorithms::OneToManyDijkstra::GetTravelTimeCalculator() {
    return timeCalculator;
}

Routing::Algorithms::OneToManyDijkstra::OneToManyDijkstra(
        std::shared_ptr<Routing::Data::GraphMemory> routingGraph, Routing::Algorithms::AlgorithmSettings settings,
        TravelCostCalculator *travelCostCalculator, TravelTimeCalculator *travelTimeCalculator)
        : Algorithm(routingGraph, settings), costCalculator(travelCostCalculator),
          timeCalculator(travelTimeCalculator) {}
