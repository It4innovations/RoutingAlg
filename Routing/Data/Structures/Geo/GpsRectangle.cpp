
#include <assert.h>
#include "GpsRectangle.h"

void Routing::GpsRectangle::Expand(float lat, float lon) {
    if (lat > this->topLeft->lat) this->topLeft->lat = lat;
    if (lat < this->bottomRight->lat) this->bottomRight->lat = lat;
    if (lon < this->topLeft->lon) this->topLeft->lon = lon;
    if (lon > this->bottomRight->lon) this->bottomRight->lon = lon;
}

void Routing::GpsRectangle::Expand(const Routing::GpsPoint &point) {
    if (point.lat > this->topLeft->lat) this->topLeft->lat = point.lat;
    if (point.lat < this->bottomRight->lat) this->bottomRight->lat = point.lat;
    if (point.lon < this->topLeft->lon) this->topLeft->lon = point.lon;
    if (point.lon > this->bottomRight->lon) this->bottomRight->lon = point.lon;
}

void Routing::GpsRectangle::Expand(const Routing::GpsRectangle &rect) {
    assert(rect.bottomRight != nullptr);
    assert(rect.topLeft != nullptr);

    this->Expand(*rect.bottomRight);
    this->Expand(*rect.topLeft);
}

void Routing::GpsRectangle::MultiplySize(float scale) {
    float width = this->bottomRight->lon - topLeft->lon;
    float height = topLeft->lat - bottomRight->lat;

    float div = width / height;
    float widthMul = div < 0.66f ? 1 / div / 1.5f : 1;
    float heightMul = div > 1.5f ? div / 1.5f : 1;

    this->topLeft->lat += height * scale * heightMul;
    bottomRight->lat -= height * scale * heightMul;
    topLeft->lon -= width * scale * widthMul;
    bottomRight->lon += width * scale * widthMul;
}
