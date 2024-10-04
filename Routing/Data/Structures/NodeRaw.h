#pragma once

#include <ostream>

#include "../Utility/Definitions.h"

//using Routing::byte;

namespace Routing {

    struct NodeRaw final {
    public:
        int id;
        int latitudeInt;
        int longitudeInt;
        unsigned char edgesOutCount;
        int edgesOutIndex;
        unsigned char edgesInCount;

        NodeRaw() {};

        NodeRaw(const NodeRaw &other) :
                id(other.id),
                latitudeInt(other.latitudeInt),
                longitudeInt(other.longitudeInt),
                edgesOutCount(other.edgesOutCount),
                edgesOutIndex(other.edgesOutIndex),
                edgesInCount(other.edgesInCount) {};

        NodeRaw &operator=(const NodeRaw &other) {
            this->id = other.id;
            this->latitudeInt = other.latitudeInt;
            this->longitudeInt = other.longitudeInt;
            this->edgesOutCount = other.edgesOutCount;
            this->edgesOutIndex = other.edgesOutIndex;
            this->edgesInCount = other.edgesInCount;
            return *this;
        }

        friend std::ostream &operator<<(std::ostream &os, const NodeRaw &node) {
            os << "ID: " << node.id << ", Lng: " << node.GetLongitude() << ", Lat: " << node.GetLatitude() <<
               ", EOCount: " << static_cast<int>(node.edgesOutCount) << ", EIndex: " << node.edgesOutIndex <<
               ", EICount" << static_cast<int>(node.edgesInCount);

            return os;
        }

        float GetLatitude(void) const { return ConvertE6IntToFloat(this->latitudeInt); };

        void SetLatitude(const float value) {
            this->latitudeInt = static_cast<int32_t>(value * this->coordinateDividor);
        };

        float GetLongitude(void) const { return ConvertE6IntToFloat(this->longitudeInt); };

        void SetLongitude(const float value) {
            this->longitudeInt = static_cast<int32_t>(value * this->coordinateDividor);
        };

    private:
        const float coordinateDividor = 1E6f; // 10^6

        inline int ConvertFloatToE6Int(const float value) const {
            return static_cast<int32_t>(value * this->coordinateDividor);
        }

        inline float ConvertE6IntToFloat(const int value) const {
            return value / this->coordinateDividor;
        }
    };
}


