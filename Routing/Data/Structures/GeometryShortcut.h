#pragma once

#include <memory>
#include <vector>

#include "Geo/GpsPoint.h"

namespace Routing {

    struct GeometryShortcut {
    public:
        int nodeId1;
        int nodeId2;

        GeometryShortcut(int id1, int id2, const std::vector<GpsPoint> &points) :
                nodeId1(id1),
                nodeId2(id2),
                points(new std::vector<GpsPoint>(points)) {

        };

        GeometryShortcut(int &&id1, int &&id2, std::vector<GpsPoint> &&points) :
                nodeId1(std::move(id1)),
                nodeId2(std::move(id2)),
                points(new std::vector<GpsPoint>(std::move(points))) {};

        std::ostream &operator<<(std::ostream &os) {
            os << "ID1: " << this->nodeId1 << ", ID2: " <<
               this->nodeId2 << ", Points: " << this->points->size();

            return os;
        }

        std::vector<GpsPoint> &GetPoints(void) const { return *this->points; };

    private:
        std::unique_ptr<std::vector<GpsPoint>> points;
    };

}


