
#define _USE_MATH_DEFINES

#include <cmath>
#include "DistanceCalculator.h"

float Routing::DistanceCalculator::GetDistance(const struct Node &n1, const struct Node &n2) {
    return static_cast<float> (DistanceCalculator::HaversineDistance(n1.GetLongitude(), n1.GetLatitude(),
                                                                     n2.GetLongitude(), n2.GetLatitude()));
}

float Routing::DistanceCalculator::GetDistance(float x1, float y1, float x2, float y2) {
    return static_cast<float> (DistanceCalculator::HaversineDistance(x1, y1, x2, y2));
}

double Routing::DistanceCalculator::GetDistance(double x1, double y1, double x2, double y2) {
    return DistanceCalculator::HaversineDistance(x1, y1, x2, y2);
}

bool Routing::DistanceCalculator::NodesInDistance(const struct Node &n1, const struct Node &n2, int length) {
    return static_cast<double> (DistanceCalculator::GetDistance(n1, n2)) <= length;
}

double Routing::DistanceCalculator::HaversineDistance(double lon1, double lat1, double lon2, double lat2) {
    const double R = 6371000;
    const double degreesToRadians = M_PI / 180;

    lon1 *= degreesToRadians;
    lat1 *= degreesToRadians;
    lon2 *= degreesToRadians;
    lat2 *= degreesToRadians;

    double dlon = lon2 - lon1;
    double dlat = lat2 - lat1;

    double a = pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double d = R * c;

    return d;
}