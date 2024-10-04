
#include "Segment.h"

Routing::Algorithms::Segment::Segment(int edgeId, int nodeId1, int nodeId2,
                                      int length, float time, bool isTollWay, std::vector<GpsPoint> &geometry)
        : SegmentBase(edgeId, time, geometry) {
    this->nodeId1 = nodeId1;
    this->nodeId2 = nodeId2;
    this->length = length;
    this->isTollWay = isTollWay;
}