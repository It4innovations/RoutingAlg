#pragma once

#include <iostream>

#include "Edge.h"

namespace Routing {

    struct Edge;

    struct Node {
    public:
        int id;
        std::vector<Edge *> edgesOut;
        std::vector<Edge *> edgesIn;
        float latitude;
        float longitude;

        Node() {};

        Node(int id, std::vector<Edge *> &&edgeOut, int latitudeInt, int longitudeInt, unsigned char edgeInCount) :
                id(id),
                edgesOut(edgeOut),
                edgesIn(std::vector<Edge *>()),
                latitude(ConvertE6IntToFloat(latitudeInt)),
                longitude(ConvertE6IntToFloat(longitudeInt)) {
            this->edgesIn.reserve(edgeInCount);
        };

        Node(const Node &other) :
                id(other.id),
                edgesOut(other.edgesOut),
                edgesIn(other.edgesIn),
                latitude(other.latitude),
                longitude(other.longitude) {};

        Node &operator=(const Routing::Node &other) {
            this->id = other.id;
            this->edgesOut = other.edgesOut;
            this->edgesIn = other.edgesIn;
            this->latitude = other.latitude;
            this->longitude = other.longitude;

            return *this;
        }

        friend std::ostream &operator<<(std::ostream &os, const Routing::Node &node) {
            os << "ID: " << node.id << ", Lng: " << node.GetLongitude() << ", Lat: " << node.GetLatitude();

            return os;
        }

        inline const std::vector<Edge *> GetEdgesIn(void) const { return this->edgesIn; };

        inline const std::vector<Edge *> GetEdgesOut(void) const { return this->edgesOut; };

        inline float GetLatitude(void) const { return this->latitude; };

        inline float GetLongitude(void) const { return this->longitude; };

    private:
        const float coordinateDividor = 1E6f; // 10^6

        inline float ConvertE6IntToFloat(const int value) const {
            return value / this->coordinateDividor;
        }

    };
}


