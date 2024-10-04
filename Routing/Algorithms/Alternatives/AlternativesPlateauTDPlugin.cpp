
#include "AlternativesPlateauTDPlugin.h"

google::dense_hash_map<int, float>
Routing::Algorithms::AlternativesPlateauTDPlugin::GetEdgesSpeedValue(int startId, int endId,
                                                                     GraphFilterGeometry plateauFilter,
                                                                     bool allFilterOff,
                                                                     int startTime) {

    VisitedNodeHashMap closedSetForth;
    closedSetForth.set_empty_key(std::numeric_limits<int>::min());
    Routing::Collections::BinaryHeap<float, int> openSetForth;

    auto startNode = this->routingGraph->GetNodeById(startId);
    auto endNode = this->routingGraph->GetNodeById(endId);

    closedSetForth[startNode.id] = VisitedNode(&startNode, startTime, true);

    openSetForth.Add(0, startNode.id);

    return DijkstraForth(openSetForth, closedSetForth, plateauFilter, allFilterOff);
}

google::dense_hash_map<int, float> Routing::Algorithms::AlternativesPlateauTDPlugin::DijkstraForth(
        Routing::Collections::BinaryHeap<float, int> &openSetForth, VisitedNodeHashMap &closedSetForth,
        GraphFilterGeometry plateauFilter, bool allFilterOff) {

    google::dense_hash_map<int, float> edgesSpeed;
    edgesSpeed.set_empty_key(std::numeric_limits<int>::min());

    while (openSetForth.Count() != 0) {
        int actualId = openSetForth.Remove();

        VisitedNode &tmp = closedSetForth[actualId];
        tmp.SetWasUsed(true);
        VisitedNode actualNode = tmp;

        const auto &edges = this->routingGraph->GetEdgesOut(actualId);

        for (auto &edge : edges) {

            Node node2 = this->routingGraph->GetEndNodeByEdge(*edge);

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

            float travelTime;
            float edgeSpeed;

            try {
                auto profile = this->storage->GetSpeedProfileByEdgeId(edge->edgeId);
                travelTime = Routing::Algorithms::TD::CalculateCostSecPrecise(*edge, actualNode.TotalCost, profile,
                                                                              this->storage);

                float startTime = actualNode.TotalCost;

                if (unlikely((int) startTime >= (secondsInWeek))) {
                    startTime = (int) startTime % secondsInWeek;
                }
                unsigned long index = (unsigned long) startTime / oneSpeedIntervalInSeconds;
                edgeSpeed = profile->at(index);
            }
            catch (Routing::Exception::ProfileNotFound e) {
                travelTime = Routing::Algorithms::TD::CalculateCostSec(*edge, edge->GetSpeed());
                edgeSpeed = (float) (edge->GetSpeed() / 3.6);
            }

            float totalCost = actualNode.TotalCost + travelTime;

            auto node2Find = closedSetForth.find(node2.id);

            if (node2Find == closedSetForth.end()) {
                closedSetForth[node2.id] = VisitedNode(&node2, actualId, edge, totalCost, travelTime, false);
                openSetForth.Add(totalCost, node2.id);
                edgesSpeed[edge->edgeId] = edgeSpeed;

            } else {
                if (!node2Find->second.WasUsed) {
                    if (node2Find->second.TotalCost > totalCost) {
                        openSetForth.UpdatePriority(totalCost, node2.id);
                        node2Find->second.Update(actualId, edge, totalCost, travelTime);
                        edgesSpeed[edge->edgeId] = edgeSpeed;
                    }
                }
            }
        }
    }

    return edgesSpeed;
}