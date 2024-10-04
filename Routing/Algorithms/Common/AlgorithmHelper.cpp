
#include "AlgorithmHelper.h"

vector<Routing::Algorithms::Segment> Routing::Algorithms::AlgorithmHelper::GetRoadsNew(
        google::dense_hash_map<int, Routing::Algorithms::VisitedNode> &closedSet, const int startNodeId,
        const int endNodeId) {
    std::vector<Segment> results;

    if (startNodeId == endNodeId) {
        return results;
    }

    if (closedSet.find(endNodeId) != closedSet.end()) {

        int actualNodeId = endNodeId;
        int totalLength = 0;
        float totalTime = 0;

        std::vector<VisitedNode> visitedList;

        do {
            const VisitedNode &actualNode = closedSet[actualNodeId];
            visitedList.push_back(actualNode);
            actualNodeId = actualNode.PreviousNodeId;
        } while (actualNodeId >= 0);

        std::reverse(visitedList.begin(), visitedList.end());

        for (std::vector<VisitedNode>::size_type i = 1; i < visitedList.size(); ++i) {
            totalTime += visitedList[i].TravelTime;
            totalLength += visitedList[i].EdgeP->length;

            std::vector<GpsPoint> points = {
                    GpsPoint(visitedList[i - 1].NodeP->latitude, visitedList[i - 1].NodeP->longitude),
                    GpsPoint(visitedList[i].NodeP->latitude, visitedList[i].NodeP->longitude)
            };

            results.emplace_back(Segment(visitedList[i].EdgeP->edgeId,
                                         visitedList[i].PreviousNodeId,
                                         visitedList[i].NodeP->id,
                                         totalLength,
                                         totalTime,
                                         false,
                                         points));
        }
    }

    return results;
}

vector<Routing::Algorithms::Segment> Routing::Algorithms::AlgorithmHelper::GetRoadsReverseNew(
        google::dense_hash_map<int, Routing::Algorithms::VisitedNode> &closedSet, const int startNodeId,
        const int endNodeId, const int startLength, const float startTime) {

    std::vector<Segment> results;

    if (startNodeId == endNodeId) {
        return results;
    }

    if (closedSet.find(endNodeId) != closedSet.end()) {

        int actualNodeId = endNodeId;
        float totalTime = startTime;
        int totalLength = startLength;

        do {
            const VisitedNode &node1 = closedSet[actualNodeId];
            const VisitedNode &node2 = closedSet[node1.PreviousNodeId];

            totalTime += node1.TravelTime;
            totalLength += node1.EdgeP->length;

            std::vector<GpsPoint> vc = {GpsPoint(node1.NodeP->latitude, node1.NodeP->longitude),
                                        GpsPoint(node2.NodeP->latitude, node2.NodeP->longitude)
            };

            results.emplace_back(Segment(node1.EdgeP->edgeId, node1.NodeP->id,
                                         node1.PreviousNodeId, totalLength, totalTime,
                                         false, vc));

            actualNodeId = node1.PreviousNodeId;

        } while (closedSet[actualNodeId].PreviousNodeId >= 0);
    }

    return results;
}

vector<Routing::Algorithms::Segment> Routing::Algorithms::AlgorithmHelper::GetRoadsBidirectionalNew(
        google::dense_hash_map<int, Routing::Algorithms::VisitedNode> &closedSetForth,
        google::dense_hash_map<int, Routing::Algorithms::VisitedNode> &closedSetBack, const int startNodeId,
        const int endNodeId, const int intersectionId) {

    auto resultsForth = AlgorithmHelper::GetRoadsNew(closedSetForth, startNodeId, intersectionId);

    int totalLengthForth = resultsForth.size() != 0 ? resultsForth.back().length : 0;
    float totalTimeForth = resultsForth.size() != 0 ? resultsForth.back().time : 0;

    auto resultsBack = AlgorithmHelper::GetRoadsReverseNew(closedSetBack, endNodeId,
                                                           intersectionId, totalLengthForth, totalTimeForth);

    resultsForth.insert(resultsForth.end(), resultsBack.begin(), resultsBack.end());

    return resultsForth;
}

vector<Routing::Algorithms::DistanceInfo> Routing::Algorithms::AlgorithmHelper::GetDistanceInfos(
        google::dense_hash_map<int, Routing::Algorithms::VisitedNode> &closedSet, const int startNodeId,
        const google::dense_hash_set<int> &endNodeIds) {

    std::vector<DistanceInfo> results;

    if (endNodeIds.size() == 0) {
        return results;
    }

    for (const int eid : endNodeIds) {
        if (eid == startNodeId) {
            results.emplace_back(DistanceInfo(startNodeId, eid, 0, 0));
            continue;
        }

        int actualNodeId = eid;
        int totalLength = 0;
        float totalTime = 0;

        do {
            const VisitedNode &visitedNode = closedSet[actualNodeId];
            actualNodeId = visitedNode.PreviousNodeId;
            totalLength += visitedNode.EdgeP->length;
            totalTime += visitedNode.TravelTime;
        } while (startNodeId != actualNodeId);

        results.emplace_back(DistanceInfo(startNodeId, eid, totalTime, totalLength));
    }

    return results;
}

RouteInfo Routing::Algorithms::AlgorithmHelper::GetRouteInfo(
        google::dense_hash_map<int, Routing::Algorithms::VisitedNode> &closedSet, const int startNodeId,
        const int endNodeId) {

    if (closedSet.find(endNodeId) == closedSet.end()) return RouteInfo(0, 0);
    if (startNodeId == endNodeId) return RouteInfo(0, 0);

    int actualNodeId = endNodeId;
    int totalLength = 0;
    float totalTime = 0;

    do {
        const VisitedNode &actualVisitedNode = closedSet[actualNodeId];
        totalTime += actualVisitedNode.TravelTime;
        totalLength += actualVisitedNode.EdgeP->length;
        actualNodeId = actualVisitedNode.PreviousNodeId;
    } while (actualNodeId != startNodeId);

    return RouteInfo(totalLength, totalTime);
}


