#include "DistanceInfo.h"

Routing::Algorithms::DistanceInfo::DistanceInfo(int nodeId1, int nodeId2, float travelTime, int travelLength) {
    NodeId1 = nodeId1;
    NodeId2 = nodeId2;
    TravelTime = travelTime;
    TravelLength = travelLength;
}

Routing::Algorithms::DistanceInfo::DistanceInfo(const DistanceInfo &other) : NodeId1(other.NodeId1),
                                                                             NodeId2(other.NodeId2),
                                                                             TravelTime(other.TravelTime),
                                                                             TravelLength(other.TravelLength) {}

float Routing::Algorithms::DistanceInfo::GetAverageSpeed() const {
    return TravelLength / TravelTime;
}