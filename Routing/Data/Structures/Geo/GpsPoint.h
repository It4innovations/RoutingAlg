#pragma once

#include <iostream>
#include <memory>
#include <limits>

namespace Routing {

    struct GpsPoint {

    public:
        float lat;
        float lon;

        GpsPoint() {};

        GpsPoint(float lat, float lon) :
                lat(lat),
                lon(lon) {};

        GpsPoint(double lat, double lon) :
                lat(static_cast<float>(lat)),
                lon(static_cast<float>(lon)) {};

        GpsPoint(int lat, int lon) :
                lat(static_cast<float>(lat)),
                lon(static_cast<float>(lon)) {};

        std::ostream &operator<<(std::ostream &os) {
            os.precision(5);
            os << this->lat << " " << this->lon;

            return os;
        }

        friend std::ostream &operator<<(std::ostream &os, const GpsPoint &point) {
            os.precision(5);
            os << point.lat << " " << point.lon;

            return os;
        }

        bool operator==(const GpsPoint &other) { return this->lat == other.lat && this->lon == other.lon; };

        bool operator!=(const GpsPoint &other) { return this->lat != other.lat || this->lon != other.lon; };

        std::unique_ptr<GpsPoint> operator-(GpsPoint &other) {
            std::unique_ptr<GpsPoint> p_gps(new GpsPoint(this->lat - other.lat,
                                                         this->lon - other.lon));

            return p_gps;
        }

        static std::unique_ptr<GpsPoint> GetEmpty(void) {
            std::unique_ptr<GpsPoint> p_gps(
                    new GpsPoint(std::numeric_limits<float>::infinity(),
                                 std::numeric_limits<float>::infinity()));

            return p_gps;
        };
    };
}


