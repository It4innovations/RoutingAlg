
#include "SegmentBase.h"

Routing::Algorithms::SegmentBase::SegmentBase(int edgeId, float time, std::vector<GpsPoint> &geometry) : edgeId(edgeId),
                                                                                                         time(time) {
    if (geometry.size() <= 1)
        throw std::length_error("Invalid geometry length");

    this->geometry.reset(new std::vector<GpsPoint>(geometry));
}

Routing::Algorithms::SegmentBase::SegmentBase(const SegmentBase &other) : edgeId(other.edgeId),
                                                                          time(other.time),
                                                                          geometry(new std::vector<GpsPoint>(
                                                                                  *other.geometry)) {}

Routing::Algorithms::SegmentBase::SegmentBase(SegmentBase &&other) : edgeId(0),
                                                                     time(0),
                                                                     geometry(nullptr) {
    this->time = other.time;
    this->edgeId = other.edgeId;
    this->geometry.reset(other.geometry.release());

    other.time = 0;
    other.edgeId = 0;
    other.geometry = nullptr;
}

std::vector<GpsPoint> &Routing::Algorithms::SegmentBase::GetGeometry(void) const { return *this->geometry; }

void Routing::Algorithms::SegmentBase::SetGeometry(std::vector<GpsPoint> &geo) {
    this->geometry.reset(new std::vector<GpsPoint>(geo));
}

bool Routing::Algorithms::SegmentBase::GeometryEquals(const std::vector<GpsPoint> &other) {
    if (geometry->size() != other.size()) return false;
    else {
        for (unsigned int i = 0; i < geometry->size(); i++) {
            if (geometry->at(i) != other.at(i)) return false;
        }
    }

    return true;
}