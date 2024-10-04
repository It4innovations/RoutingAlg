#pragma once

#include "memory"
#include <ostream>

namespace Routing {

    struct GpsDPoint {
        double lat;
        double lon;

        GpsDPoint(double Lat, double Lon) :
                lat(Lat),
                lon(Lon) {};

        std::ostream &operator<<(std::ostream &os) {
            os.precision(5);
            os << this->lat << " " << this->lon;

            return os;
        }

        //TODO equal

        bool operator==(const GpsDPoint &other) { return this->lat == other.lat && this->lon == other.lon; };

        bool operator!=(const GpsDPoint &other) { return this->lat != other.lat || this->lon != other.lon; };

        static std::unique_ptr<GpsDPoint> GetEmpty() {
            std::unique_ptr<GpsDPoint> p_gps(new GpsDPoint(std::numeric_limits<double>::infinity(),
                                                           std::numeric_limits<float>::infinity()));

            return p_gps;
        };
    };
}


