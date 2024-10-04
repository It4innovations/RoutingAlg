#pragma once

#include <cmath>

namespace GPSDistanceCalculation {

    class GPSDistanceCalculation {
    private:
        double Deg2rad(double deg) {
            return (deg * M_PI / 180);
        }

    public:

        double DistanceEarth(double lat1d, double lon1d, double lat2d, double lon2d) {
            double lat1r, lon1r, lat2r, lon2r, u, v;
            double earthRadiusKm = 6371.0;
            lat1r = Deg2rad(lat1d);
            lon1r = Deg2rad(lon1d);
            lat2r = Deg2rad(lat2d);
            lon2r = Deg2rad(lon2d);
            u = sin((lat2r - lat1r) / 2);
            v = sin((lon2r - lon1r) / 2);
            return 2.0 * earthRadiusKm * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
        }
    };
}

