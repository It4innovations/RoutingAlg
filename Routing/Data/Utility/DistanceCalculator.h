#pragma once

#include <cmath>

#include "../Structures/Node.h"

namespace Routing {

    class DistanceCalculator {
    public:
        static float GetDistance(const struct Node &n1, const struct Node &n2);

        static float GetDistance(float x1, float y1, float x2, float y2);

        static double GetDistance(double x1, double y1, double x2, double y2);

        static float GetDistance(const struct GpsPoint &g1, const struct GpsPoint &g2);

        static double GetDistance(const struct GpsDPoint &g1, const struct GpsDPoint &g2);

        static bool NodesInDistance(const struct Node &n1, const struct Node &n2, int length);

        static double HaversineDistance(double lon1, double lat1, double lon2, double lat2);
    };
}


