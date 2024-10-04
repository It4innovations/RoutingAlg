#pragma once

#include <vector>
#include <ostream>

#include "../../Data/Structures/Geo/GpsPoint.h"

using Routing::GpsPoint;

namespace Routing {

    namespace Algorithms {

        //TODO usage
        class SegmentBase {
        public:
            int edgeId;
            float time;
            std::unique_ptr<std::vector<GpsPoint>> geometry;

            //used by GetSegmentByEdge + hash_set_map
            SegmentBase(int edgeId) :
                    edgeId(edgeId),
                    time(0.0),
                    geometry(new std::vector<GpsPoint>()) {};

            SegmentBase(int edgeId, float time, std::vector<GpsPoint> &geometry);

            SegmentBase(const SegmentBase &other);

            SegmentBase(SegmentBase &&other);

            SegmentBase &operator=(const SegmentBase &other) {
                this->edgeId = other.edgeId;
                this->time = other.time;
                this->geometry.reset(new std::vector<GpsPoint>(*other.geometry));

                return *this;
            }

            SegmentBase &operator=(SegmentBase &&other) {
                this->edgeId = other.edgeId;
                this->time = other.time;
                this->geometry.reset(other.geometry.release());

                other.edgeId = 0;
                other.time = 0;
                other.geometry = nullptr;

                return *this;
            }

            virtual bool operator==(const SegmentBase &other) {
                return this->edgeId == other.edgeId && this->time == other.time && GeometryEquals(*other.geometry);
            }

            std::vector<GpsPoint> &GetGeometry(void) const;;

            void SetGeometry(std::vector<GpsPoint> &geo);;

            friend std::ostream &operator<<(std::ostream &os, const SegmentBase &seg) {
                os << "ID: " << seg.edgeId << ", T:" << seg.time << ", G: " << seg.GetGeometry().size();

                return os;
            }

            virtual ~SegmentBase() {};

        protected:
            bool GeometryEquals(const std::vector<GpsPoint> &other);
        };
    }
}


