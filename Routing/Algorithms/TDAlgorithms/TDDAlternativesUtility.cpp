
#include "TDDAlternativesUtility.h"

std::vector<std::vector<Routing::Algorithms::Segment>> Routing::Algorithms::TD::TDDAlternativesUtility::GetRoute(
        google::dense_hash_set<int> betweennessNodes,
        std::vector<std::vector<int>> alternativesNodes,
        int startId, int endId, int startTime) {

    auto middleNodes = FindMiddleNodes(betweennessNodes, alternativesNodes, startId);

    auto results = FindRoutes(middleNodes, startId, endId, startTime);

    return results;
}

std::vector<int>
Routing::Algorithms::TD::TDDAlternativesUtility::FindMiddleNodes(google::dense_hash_set<int> &betweennessNodes,
                                                                 std::vector<std::vector<int>> &alternativesNodes,
                                                                 int startId) {

    std::vector<int> middleNodes;
    for (auto alternative : alternativesNodes) {
        bool sameLineOfNodes = false;
        int lastIdNode = startId;

        for (auto nodeID : alternative) {
            if (betweennessNodes.find(nodeID) != betweennessNodes.end()) {
                if (!sameLineOfNodes) {
                    std::cout << nodeID << ", ";
                    if (!NodeInVector(middleNodes, nodeID)) {
                        middleNodes.push_back(nodeID); //node - nodeID
                    }
                    sameLineOfNodes = true;
                }
            } else {
                if (sameLineOfNodes) {
                    std::cout << lastIdNode << ", ";
                    if (!NodeInVector(middleNodes, lastIdNode)) {
                        middleNodes.push_back(lastIdNode); //node - lastIdNode
                    }
                }
                sameLineOfNodes = false;
            }

            lastIdNode = nodeID;
        }
    }

    return middleNodes;
}

std::vector<std::vector<Routing::Algorithms::Segment>> Routing::Algorithms::TD::TDDAlternativesUtility::FindRoutes
        (std::vector<int> &middleNodes, int startId, int endId, int startTime) {

    std::vector<std::vector<Segment>> results;

    for (auto middleNode : middleNodes) {
        auto firstPartResult = this->dijkstraTD->GetRoute(startId, middleNode, startTime);
        int firstPartTime;
        int firstPartLength;
        bool onlyOnePart = false;
        if (!firstPartResult.empty()) {
            firstPartTime = (int) firstPartResult.at(firstPartResult.size() - 1).time;
            firstPartLength = firstPartResult.at(firstPartResult.size() - 1).length;
        } else {
            firstPartTime = 0;
            firstPartLength = 0;
            onlyOnePart = true;
        }

        auto secondPartResult = this->dijkstraTD->GetRoute(middleNode, endId, startTime + firstPartTime);

        if (!onlyOnePart) {
            for (auto segment : secondPartResult) {
                segment.time = segment.time + firstPartTime;
                segment.length = segment.length + firstPartLength;
            }
        }

        std::vector<Segment> result = firstPartResult;
        result.insert(result.end(), secondPartResult.begin(), secondPartResult.end());

        if (IsUniqueRoute(results, result)) {
            results.push_back(result);
        }
    }

    return results;
}

bool Routing::Algorithms::TD::TDDAlternativesUtility::IsUniqueRoute(
        std::vector<std::vector<Routing::Algorithms::Segment>> &results,
        std::vector<Routing::Algorithms::Segment> newRoute) {

    if (newRoute.empty()) {
        return false;
    }

    if (results.empty()) {
        return true;
    }

    for (auto route : results) {
        int countSameSegments = 0;
        int index = 0;
        auto maxIndex = (int) newRoute.size();

        for (auto segment : route) {
            if (index < maxIndex) {
                if (segment.nodeId1 != newRoute.at(index).nodeId1) {
                    break;
                }
                index++;
            } else {
                break;
            }
        }

        countSameSegments = index;
        index = (int) newRoute.size() - 1;

        for (int i = (int) route.size() - 1; i >= 0; i--) {
            if (index >= 0) {
                if (route.at(i).nodeId1 != newRoute.at(index).nodeId1) {
                    break;
                }
                index--;
                countSameSegments++;
            } else {
                break;
            }
        }

        if ((double) route.size() / countSameSegments <= 1.25) {
            return false;
        }
    }

    return true;
}

bool Routing::Algorithms::TD::TDDAlternativesUtility::NodeInVector(std::vector<int> &middleNodes, int node) {
    for (auto middleNode : middleNodes) {
        if (middleNode == node) {
            return true;
        }
    }

    return false;
}