#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
#include "AlternativesPlateauAlgorithm.h"

#define TENPERCENT /10
#define FIVEPERCENT /20

std::vector<std::vector<Segment>> Routing::Algorithms::AlternativesPlateauAlgorithm::GetRoutes(
        int startId, int endId, unsigned int maxRoutes, bool multiThreading, int startTime) {

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
        return std::vector<std::vector<Segment>>();
    }

    int countThreads = 2;

    if (!multiThreading) {
        countThreads = 1;
    }

    bool longDistance = false;

    GetStartInfo(startNode, endNode, longDistance);

    auto filterGeometryRootForth = InitiateFilters(startNode, endNode, startNode, longDistance);
    auto filterGeometryRootBack = InitiateFilters(startNode, endNode, endNode, longDistance);

    closedSetForth[startNode.id] = VisitedNode(&startNode, 0, true);

    openSetForth.Add(0, startNode.id);

    closedSetBack[endNode.id] = VisitedNode(&endNode, 0, true);

    openSetBack.Add(0, endNode.id);

    if (alternativesTDPluginOn) {
        std::clock_t start;
        double duration;
        start = std::clock();

        auto filterGeometryRootTD = InitiateFilters(startNode, endNode, startNode, longDistance);

        Routing::Algorithms::AlternativesPlateauTDPlugin alternativesPlateauTDPlugin(this->routingGraph,
                                                                                     this->profileStorage,
                                                                                     this->settings);

        edgesSpeed = alternativesPlateauTDPlugin.GetEdgesSpeedValue(startId, endId, filterGeometryRootTD,
                                                                    settings.filterSettings.allFilterOff, startTime);

        duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;

        filterGeometryRootTD.DeleteAllFilter();

        std::cout << "Time of TD part: " << duration << std::endl << std::endl;
    }

#pragma omp parallel sections num_threads(countThreads)
    {
#pragma omp section
        {
            this->DijkstraForth(openSetForth, closedSetForth, filterGeometryRootForth,
                                settings.filterSettings.allFilterOff);
        }
#pragma omp section
        {
            this->DijkstraBack(openSetBack, closedSetBack, filterGeometryRootBack,
                               settings.filterSettings.allFilterOff);
        }
    }

    filterGeometryRootForth.DeleteAllFilter();
    filterGeometryRootBack.DeleteAllFilter();

    auto solutions = FindPlateaus(closedSetForth, closedSetBack, startId, endId, settings.filterSettings.typeOfFilter,
                                  multiThreading);


#ifndef NDEBUG
    cout << "Alternative solutions: " << solutions.size() << endl;
#endif

    std::vector<std::vector<Segment>> results;
    std::sort(solutions.begin(), solutions.end(), [](const RouteSolution &info1, const RouteSolution &info2) {
        return (info1.RouteInfo.Time < info2.RouteInfo.Time);
    });

    if (useSimilarityParameter) {
        maxRoutes = maxRoutes * similarityParMultiplyConst;
    }

    for (unsigned int i = 0; (i < maxRoutes || (alternativesRouteTDRecalculateOn && i <= altCountsForRecalculation)) &&
                             i < solutions.size(); ++i) {

        results.push_back(AlgorithmHelper::GetRoadsBidirectionalNew(closedSetForth, closedSetBack,
                                                                    startId, endId, solutions[i].IntersectionId));
    }

    if (useSimilarityParameter) {
        maxRoutes = maxRoutes / similarityParMultiplyConst;
        results = AlternativesSimilarityParameter(results, similarityPercent, maxRoutes);
    }


    if (alternativesRouteTDRecalculateOn && this->profileStorage != nullptr) {
        return AlternativesRecalculate(startTime, maxRoutes, results);
    }

    return results;
}

void
Routing::Algorithms::AlternativesPlateauAlgorithm::GetStartInfo(Node &startNode, Node &endNode, bool &longDistance) {
    int highestFRCoutForth = 7;
    int highestFRCinBack = 7;

    for (auto item : startNode.edgesOut) {
        auto RFC = (int) item->GetFuncClass();
        if (RFC < highestFRCoutForth) {
            highestFRCoutForth = RFC;
        }
    }

    for (auto item : endNode.edgesIn) {
        auto RFC = (int) item->GetFuncClass();
        if (RFC < highestFRCinBack) {
            highestFRCinBack = RFC;
        }
    }

#ifndef NDEBUG
    //std::cout << "Forth FRC: " << highestFRCoutForth << " Back FRC: " << highestFRCinBack << std::endl;
#endif

    GPSDistanceCalculation::GPSDistanceCalculation distanceCalc;

    auto distance = distanceCalc.DistanceEarth((double) startNode.GetLatitude(), (double) startNode.GetLongitude(),
                                               (double) endNode.GetLatitude(), (double) endNode.GetLongitude());
#ifndef NDEBUG
    //std::cout << "Distance: " << distance << " km" << std::endl;
#endif

    if (distance > settings.filterSettings.longDistanceValue) {
        longDistance = true;
    }
}

Routing::Algorithms::GraphFilterGeometry Routing::Algorithms::AlternativesPlateauAlgorithm::InitiateFilters(
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

void Routing::Algorithms::AlternativesPlateauAlgorithm::DijkstraForth(BinHeap &openSetForth,
                                                                      VisitedNodeHashMap &closedSetForth,
                                                                      GraphFilterGeometry plateauFilter,
                                                                      bool allFilterOff) {
    //allFilterOff = true;

    while (openSetForth.Count() != 0) {
        int actualId = openSetForth.Remove();

        VisitedNode &tmp = closedSetForth[actualId];
        tmp.SetWasUsed(true);
        VisitedNode actualNode = tmp;

        const auto &edges = this->routingGraph->GetEdgesOut(actualId);

        for (const auto &edge : edges) {

            const Node &node2 = this->routingGraph->GetEndNodeByEdge(*edge);

            bool filtersPass = true;

            if (!allFilterOff) {
                plateauFilter.SetAllFiltersData(node2, *edge);
                plateauFilter.Handle(filtersPass);
            }

            if (!filtersPass) {
                continue;
            }

            if (!this->IsAccessible(edge->GetEdgeData())) {
                continue;
            }

            float speedMPS;

            if (alternativesTDPluginOn) {
                if (edgesSpeed.find(edge->edgeId) == edgesSpeed.end()) {
                    speedMPS = (this->settings.maxVelocity < edge->GetSpeed() ?
                            this->settings.maxVelocity : edge->GetSpeed()) * Constants::OneDiv3P6;
                } else {
                    float speed = edgesSpeed.find(edge->edgeId)->second;
                    speedMPS = ((this->settings.maxVelocity * Constants::OneDiv3P6) < speed ?
                                (this->settings.maxVelocity * Constants::OneDiv3P6) : speed);
                }
            } else {
                speedMPS = (this->settings.maxVelocity < edge->GetSpeed()
                        ? this->settings.maxVelocity : edge->GetSpeed()) * Constants::OneDiv3P6;
            }

            float travelTime = timeCalculator->GetTravelTime(edge->length, speedMPS);
            float totalCost = actualNode.TotalCost + costCalculator->GetTravelCost(edge->length,
                                                                                   travelTime, edge->GetFuncClass());

            auto node2Find = closedSetForth.find(node2.id);

            if (node2Find == closedSetForth.end()) {
                closedSetForth[node2.id] = VisitedNode(&node2, actualId, edge, totalCost, travelTime, false);
                openSetForth.Add(totalCost, node2.id);

            } else if (!node2Find->second.WasUsed) {
                if (node2Find->second.TotalCost > totalCost) {
                    openSetForth.UpdatePriority(totalCost, node2.id);
                    node2Find->second.Update(actualId, edge, totalCost, travelTime);
                }
            }
        }
    }
}

void Routing::Algorithms::AlternativesPlateauAlgorithm::DijkstraBack(BinHeap &openSetBack,
                                                                     VisitedNodeHashMap &closedSetBack,
                                                                     GraphFilterGeometry plateauFilter,
                                                                     bool allFilterOff) {

    //allFilterOff = true;

    while (openSetBack.Count() != 0) {
        int actualId = openSetBack.Remove();

        VisitedNode &tmp = closedSetBack[actualId];
        tmp.SetWasUsed(true);
        VisitedNode actualNode = tmp;

        const auto &edges = this->routingGraph->GetEdgesIn(actualId);

        for (const auto &edge : edges) {

            const Node &node2 = this->routingGraph->GetStartNodeByEdge(*edge);

            bool filtersPass = true;

            if (!allFilterOff) {
                plateauFilter.SetAllFiltersData(node2, *edge);
                plateauFilter.Handle(filtersPass);
            }

            if (!filtersPass) {
                continue;
            }

            if (!this->IsAccessible(edge->GetEdgeData())) {
                continue;
            }

            float speedMPS;

            if (alternativesTDPluginOn) {
                if (edgesSpeed.find(edge->edgeId) == edgesSpeed.end()) {
                    speedMPS = (this->settings.maxVelocity < edge->GetSpeed()
                            ? this->settings.maxVelocity : edge->GetSpeed()) * Constants::OneDiv3P6;
                } else {
                    float speed = edgesSpeed.find(edge->edgeId)->second;
                    speedMPS = ((this->settings.maxVelocity * Constants::OneDiv3P6) < speed ?
                                (this->settings.maxVelocity * Constants::OneDiv3P6) : speed);
                }
            } else {
                speedMPS = (this->settings.maxVelocity < edge->GetSpeed() ?
                        this->settings.maxVelocity : edge->GetSpeed()) * Constants::OneDiv3P6;
            }

            float travelTime = timeCalculator->GetTravelTime(edge->length, speedMPS);
            float totalCost = actualNode.TotalCost +
                              costCalculator->GetTravelCost(edge->length, travelTime, edge->GetFuncClass());

            auto node2Find = closedSetBack.find(node2.id);

            if (node2Find == closedSetBack.end()) {
                closedSetBack[node2.id] = VisitedNode(&node2, actualId, edge, totalCost, travelTime, false);
                openSetBack.Add(totalCost, node2.id);

            } else if (!node2Find->second.WasUsed) {
                if (node2Find->second.TotalCost > totalCost) {
                    openSetBack.UpdatePriority(totalCost, node2.id);
                    node2Find->second.Update(actualId, edge, totalCost, travelTime);
                }
            }
        }
    }
}

std::vector<Routing::Algorithms::RouteSolution> Routing::Algorithms::AlternativesPlateauAlgorithm::FindPlateaus(
        VisitedNodeHashMap &closedSetForth, VisitedNodeHashMap &closedSetBack, int startId, int endId,
        GraphFilterGeometry::FilterType typeOfFilter, bool multiThreading) {

    bool firstSolution = true;
    float solutionTimeLock = 0;
    int intersectionId;

    HashSet processedNodeIds;
    processedNodeIds.set_empty_key(std::numeric_limits<int>::min());
    std::vector<RouteSolution> solutions;

    for (std::pair<const int, Routing::Algorithms::VisitedNode> intersectionForthNode : closedSetForth) {
        intersectionId = intersectionForthNode.first;

        if (firstSolution) {
            if ((closedSetForth.find(startId) != closedSetForth.end()) &&
                (closedSetForth.find(endId) != closedSetForth.end())) {
                if ((closedSetBack.find(startId) != closedSetBack.end()) &&
                    (closedSetBack.find(endId) != closedSetBack.end())) {
                    intersectionId = startId;
                    auto startNode = closedSetForth.find(startId);
                    intersectionForthNode.second = startNode->second;
                    processedNodeIds.insert(intersectionForthNode.first);
                }
            }
        }


        if (processedNodeIds.find(intersectionId) != processedNodeIds.end()) {
            continue;
        }

        processedNodeIds.insert(intersectionId);

        const auto &intersectionBackNode = closedSetBack.find(intersectionId);
        if (intersectionBackNode != closedSetBack.end()) {
            int plateauLength = 0;
            float plateauTime = 0;

            if (intersectionForthNode.second.PreviousNodeId == intersectionBackNode->second.PreviousNodeId) {
                continue;
            }

            ForthPartPlateau(closedSetForth, closedSetBack, processedNodeIds, intersectionId,
                             intersectionBackNode->second.PreviousNodeId, plateauLength, plateauTime);

            BackPartPlateau(closedSetForth, closedSetBack, processedNodeIds, intersectionId,
                            intersectionForthNode.second.PreviousNodeId, plateauLength, plateauTime);


            if (plateauTime > 0 && plateauTime > solutionTimeLock) {
                float solutionTime;

                if (firstSolution) {
                    firstSolution = false;
                    AddAlternativeSolution(closedSetForth, closedSetBack, startId, endId, intersectionId, plateauLength,
                                           plateauTime, solutions, solutionTime, multiThreading);
                    if (typeOfFilter == Routing::Algorithms::GraphFilterGeometry::NotSet) {
                        solutionTimeLock = solutionTime FIVEPERCENT;
                    } else {
                        solutionTimeLock = solutionTime TENPERCENT;
                    }

                } else {
                    AddAlternativeSolution(closedSetForth, closedSetBack, startId, endId, intersectionId, plateauLength,
                                           plateauTime, solutions, solutionTime, multiThreading);
                }
            }
        }
    }

    return solutions;
}

void Routing::Algorithms::AlternativesPlateauAlgorithm::ForthPartPlateau(VisitedNodeHashMap &closedSetForth,
                                                                         VisitedNodeHashMap &closedSetBack,
                                                                         HashSet &processedNodeIds, int actualId,
                                                                         int nextId, int &plateauLength,
                                                                         float &plateauTime) {

    while (nextId != -1) {

        const auto &nextForthNode = closedSetForth.find(nextId);

        if (nextForthNode != closedSetForth.end()) {
            if (nextForthNode->second.PreviousNodeId == actualId) {
                processedNodeIds.insert(nextId);

                plateauLength += nextForthNode->second.EdgeP->length;
                plateauTime += nextForthNode->second.TravelTime;

                actualId = nextId;
                const auto &nextBackNode = closedSetBack.find(nextId);
                nextId = nextBackNode->second.PreviousNodeId;

            } else {
                break;
            }

        } else {
            break;
        }
    }
}

void Routing::Algorithms::AlternativesPlateauAlgorithm::BackPartPlateau(VisitedNodeHashMap &closedSetForth,
                                                                        VisitedNodeHashMap &closedSetBack,
                                                                        HashSet &processedNodeIds, int actualId,
                                                                        int previousId, int &plateauLength,
                                                                        float &plateauTime) {

    while (previousId != -1) {

        const auto &previousBackNode = closedSetBack.find(previousId);

        if (previousBackNode != closedSetBack.end()) {
            if (previousBackNode->second.PreviousNodeId == actualId) {
                processedNodeIds.insert(previousId);

                plateauLength += previousBackNode->second.EdgeP->length;
                plateauTime += previousBackNode->second.TravelTime;

                actualId = previousId;
                const auto &previousForthNode = closedSetForth.find(previousId);
                previousId = previousForthNode->second.PreviousNodeId;

            } else {
                break;
            }

        } else {
            break;
        }
    }
}

void Routing::Algorithms::AlternativesPlateauAlgorithm::AddAlternativeSolution(
        VisitedNodeHashMap &closedSetForth, VisitedNodeHashMap &closedSetBack, int startId, int endId,
        int intersectionId,
        int plateauLength, float plateauTime, std::vector<Routing::Algorithms::RouteSolution> &solutions,
        float &solutionTime, bool multiThreading) {

    Routing::Algorithms::RouteInfo routeInfoForth(0, 0);

    Routing::Algorithms::RouteInfo routeInfoBack(0, 0);

    int countThreads = 2;

    if (!multiThreading) {
        countThreads = 1;
    }

#pragma omp parallel sections num_threads(countThreads)
    {
#pragma omp section
        {
            routeInfoForth = AlgorithmHelper::GetRouteInfo(closedSetForth, startId, intersectionId);
        }
#pragma omp section
        {
            routeInfoBack = AlgorithmHelper::GetRouteInfo(closedSetBack, endId, intersectionId);
        }
    }

    solutions.emplace_back(RouteSolution(intersectionId, RouteInfo(
            routeInfoForth.Length + routeInfoBack.Length - plateauLength,
            routeInfoForth.Time + routeInfoBack.Time - plateauTime)));

    solutionTime = routeInfoForth.Time + routeInfoBack.Time;
}

void Routing::Algorithms::AlternativesPlateauAlgorithm::RoutesRecalculateBySpeedProfile(
        std::vector<std::vector<Segment>> &results,
        int startTime) {
    for (auto &route : results) {
        std::cout << "Route (Len: " << route.back().length << ") Time: " << route.back().time;
        Routing::Algorithms::TD::CalculateRoadCostByProfile(route, startTime, this->routingGraph, this->profileStorage);
        std::cout << "   New: " << (route.back().time - startTime) << std::endl;
    }
}

std::vector<std::vector<Segment>> Routing::Algorithms::AlternativesPlateauAlgorithm::AlternativesRecalculate(
        int startTime, unsigned int maxRoutes, std::vector<std::vector<Segment>> results) {

    RoutesRecalculateBySpeedProfile(results, startTime);
    std::sort(results.begin(), results.end(), [](const std::vector<Segment> &info1, const std::vector<Segment> &info2) {
        return (info1.back().time < info2.back().time);
    });

    unsigned int count = maxRoutes > altCountsForRecalculation ? altCountsForRecalculation : maxRoutes;
    count = count > results.size() ? (int) results.size() : count;
    results.resize(count);

    std::cout << std::endl << "Resize + Sort" << std::endl;

    for (auto item : results) {
        std::cout << "Time: " << item.back().time - startTime << "   Len: " << item.back().length << std::endl;
    }

    return results;
}

std::vector<std::vector<Segment>> Routing::Algorithms::AlternativesPlateauAlgorithm::AlternativesSimilarityParameter(
        std::vector<std::vector<Segment>> &results, double percentageSimilarity, int countResutls) {
    if (results.empty()) {
        return results;
    }

    int countCorrectResults = 1;
    double percentageSimMaxLenght = 150;
    std::vector<std::vector<Segment>> newResults;
    std::vector<double> newResultsLenght;
    newResults.push_back(results[0]);

    if (results[0].empty()) {
        return results;
    }

    double len = results[0][(results[0].size()) - 1].length;

    newResultsLenght.push_back(len);

    for (int i = 1; (i < (int) results.size()) && (countCorrectResults != countResutls); i++) {
        double lenCurrentPath = results[i][(results[i].size()) - 1].length;;
        bool addResult = true;
        int indexNewResults = 0;

        for (auto &res : newResults) {
            double lenOfSameRoad = 0;

            for (int j = 0; (j < (int) res.size()) && (j < (int) results[i].size()); j++) {
                if (res[j].edgeId != results[i][j].edgeId) {
                    lenOfSameRoad = results[i][j - 1].length; //at least start point have to be same
                    break;
                }
            }

            int indexBackNewResults = (int) res.size() - 1;
            int indexBackResults = (int) results[i].size() - 1;

            while (indexBackNewResults >= 0 && indexBackResults >= 0) {
                if (res[indexBackNewResults].edgeId != results[i][indexBackResults].edgeId) {
                    lenOfSameRoad += lenCurrentPath - results[i][indexBackResults].length;
                    break;
                }

                indexBackNewResults--;
                indexBackResults--;
            }

            double lenOfDiffrendRoad = newResultsLenght[indexNewResults] - lenOfSameRoad;

            if (((lenOfDiffrendRoad / newResultsLenght[indexNewResults]) * 100) < percentageSimilarity) {
                addResult = false;
                break;
            }

            double lenDiffrendAlternative = lenCurrentPath - lenOfSameRoad;

            if ((lenDiffrendAlternative / lenOfDiffrendRoad) * 100 > percentageSimMaxLenght) {
                addResult = false;
                break;
            }

            indexNewResults++;
        }

        if (addResult) {
            newResults.push_back(results[i]);
            newResultsLenght.push_back(lenCurrentPath);
            countCorrectResults++;
        }
    }

    return newResults;
}

Routing::Algorithms::AlternativesPlateauAlgorithm::AlternativesPlateauAlgorithm(
        std::shared_ptr<GraphMemory> routingGraph, Routing::Algorithms::AlgorithmSettings settings,
        TravelCostCalculator *travelCostCalculator, TravelTimeCalculator *travelTimeCalculator)
        :
        AlternativesAlgorithm(std::move(routingGraph), settings, travelCostCalculator, travelTimeCalculator) {
    this->alternativesTDPluginOn = false;
    this->alternativesRouteTDRecalculateOn = false;
    this->altCountsForRecalculation = settings.altCountsForRecalculation;
    this->useSimilarityParameter = settings.useSimilarityParameter;
    this->similarityParMultiplyConst = settings.similarityParMultiplyConst;
    this->similarityPercent = settings.similarityPercent;
}

Routing::Algorithms::AlternativesPlateauAlgorithm::AlternativesPlateauAlgorithm(
        std::shared_ptr<GraphMemory> routingGraph,
        std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5> storage,
        Routing::Algorithms::AlgorithmSettings settings, TravelCostCalculator *travelCostCalculator,
        TravelTimeCalculator *travelTimeCalculator)
        :
        AlternativesAlgorithm(std::move(routingGraph), settings, travelCostCalculator, travelTimeCalculator) {
    this->profileStorage = move(storage);
    this->alternativesTDPluginOn = settings.alternativesTDPluginOn;
    this->alternativesRouteTDRecalculateOn = settings.alternativesRouteTDRecalculateOn;
    this->altCountsForRecalculation = settings.altCountsForRecalculation;
    this->useSimilarityParameter = settings.useSimilarityParameter;
    this->similarityParMultiplyConst = settings.similarityParMultiplyConst;
    this->similarityPercent = settings.similarityPercent;
}

#pragma clang diagnostic pop