#pragma once

#include <vector>
#include <ostream>

#include "SegmentBase.h"

namespace Routing {

    namespace Algorithms {

        class Segment : public SegmentBase {
        public:
            int nodeId1;
            int nodeId2;
            int length;
            bool isTollWay;
            int speed;

            // Empty segment for hash_map_set
            Segment() :
                    SegmentBase(0) {};

            //TODO geometry
            Segment(int edgeId, int nodeId1, int nodeId2,
                    int length, float time, bool isTollWay, std::vector<GpsPoint> &geometry);

            //TODO tmp used only GetSegmentByEdge
            Segment(int edgeId, int nodeId1, int nodeId2, int length) :
                    SegmentBase(edgeId),
                    nodeId1(nodeId1),
                    nodeId2(nodeId2),
                    length(length),
                    isTollWay(false) {}

            Routing::Algorithms::Segment &operator=(const Segment &segment) {
                if (&segment == this) {
                    return *this;
                }

                this->nodeId1 = segment.nodeId1;
                this->nodeId2 = segment.nodeId2;
                this->length = segment.length;
                this->isTollWay = segment.isTollWay;

                return *this;
            }

            // Hidden friend function for equality comparison
            friend bool operator==(const Segment& lhs, const Segment& rhs) {
                return lhs.edgeId == rhs.edgeId &&
                       lhs.time == rhs.time &&
                       lhs.GeometryEquals(*rhs.geometry) &&
                       lhs.nodeId1 == rhs.nodeId1 &&
                       lhs.nodeId2 == rhs.nodeId2 &&
                       lhs.length == rhs.length &&
                       lhs.isTollWay == rhs.isTollWay;
            }
            
            // Override the base class equality operator
            bool operator==(const SegmentBase& other) const override {
                const auto* otherSegment = dynamic_cast<const Segment*>(&other);
                return otherSegment && (*this == *otherSegment);
            }

            friend std::ostream &operator<<(std::ostream &os, const Segment &seg) {
                os << "N1 " << seg.nodeId1 << " EI " << seg.edgeId << " N2 " << seg.nodeId2; // ", L: "
                //<< seg.length << ", T:" << seg.time << ", G: " << seg.GetGeometry().size();

                return os;
            }
        };
    }
}
