
#include "StaticRoutingAlgorithm.h"

//TODO start state? why not start ID or node
std::unique_ptr<Routing::Algorithms::Result> Routing::Algorithms::StaticRoutingAlgorithm::GetResult(
        int startId, int endId, Routing::CostCalcType costType) {

    TimeWatch watch(true);

    auto route = this->GetRoute(startId, endId, costType);

    if (route.size() == 0) {
        return nullptr;
    }

    watch.Stop();

    long baseTime = watch.GetElapsed();

    return std::unique_ptr<Result>(new Result(route, route.back().time, route.back().length, baseTime));
}

void Routing::Algorithms::StaticRoutingAlgorithm::GetStartInfo(Node startNode, Node endNode, bool &longDistance) {

    GPSDistanceCalculation::GPSDistanceCalculation distanceCalc;

    auto distance = distanceCalc.DistanceEarth((double) startNode.GetLatitude(), (double) startNode.GetLongitude(),
                                               (double) endNode.GetLatitude(), (double) endNode.GetLongitude());
#ifndef NDEBUG
    std::cout << "Distance: " << distance << " km" << std::endl;
#endif

    if (distance > settings.filterSettings.longDistanceValue) {
        longDistance = true;
    }
}

Routing::Algorithms::GraphFilterGeometry Routing::Algorithms::StaticRoutingAlgorithm::InitiateFilters(
        Node startNode, Node endNode, Node startPositionNode, bool longDistance) {

    if (!settings.filterSettings.allFilterOff) {
        GraphFilterGeometry filterGeometryRoot(settings.filterSettings.typeOfFilter, startNode, endNode);

        if (settings.filterSettings.hierarchyStartPositionFilterOn) {
            GraphFilterHierarchy *filterHierarchy = nullptr;
            GraphFilterGeometryStartLocation *filterGeometryStartLocation = nullptr;

            if (longDistance) {
                filterHierarchy = new GraphFilterHierarchy(settings.filterSettings.minFRC,
                                                           settings.filterSettings.maxFRC);
                filterGeometryStartLocation = new GraphFilterGeometryStartLocation(startPositionNode,
                                                                                   settings.filterSettings.firstHierarchyJumpLongDistance,
                                                                                   settings.filterSettings.secondHierarchyJumpLongDistance,
                                                                                   settings.filterSettings.firstHierarchyJumpFRC,
                                                                                   settings.filterSettings.secondHierarchyJumpFRC,
                                                                                   filterHierarchy);

            } else {
                filterHierarchy = new GraphFilterHierarchy(settings.filterSettings.minFRC,
                                                           settings.filterSettings.maxFRC);
                filterGeometryStartLocation = new GraphFilterGeometryStartLocation(startPositionNode,
                                                                                   settings.filterSettings.firstHierarchyJumpDistance,
                                                                                   settings.filterSettings.secondHierarchyJumpDistance,
                                                                                   settings.filterSettings.firstHierarchyJumpFRC,
                                                                                   settings.filterSettings.secondHierarchyJumpFRC,
                                                                                   filterHierarchy);
            }

            filterGeometryRoot.AddFilter(filterHierarchy);
            filterGeometryRoot.AddFilter(filterGeometryStartLocation);

        }

        return filterGeometryRoot;
    }

    GraphFilterGeometry filterGeometryRoot(GraphFilterGeometry::FilterType::NotSet, startNode, endNode);
    return filterGeometryRoot;
}

Routing::Algorithms::TravelCostCalculator *Routing::Algorithms::StaticRoutingAlgorithm::GetTravelCostCalculator() {
    return costCalculator;
}

Routing::Algorithms::TravelTimeCalculator *Routing::Algorithms::StaticRoutingAlgorithm::GetTravelTimeCalculator() {
    return timeCalculator;
}

Routing::Algorithms::StaticRoutingAlgorithm::StaticRoutingAlgorithm(std::shared_ptr<GraphMemory> routingGraph,
                                                                    Routing::Algorithms::AlgorithmSettings settings,
                                                                    TravelCostCalculator *travelCostCalculator,
                                                                    TravelTimeCalculator *travelTimeCalculator)
        : Algorithm(move(routingGraph), settings), costCalculator(travelCostCalculator),
          timeCalculator(travelTimeCalculator) {}