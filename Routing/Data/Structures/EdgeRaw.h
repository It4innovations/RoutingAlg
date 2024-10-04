#pragma once

#include <sys/types.h>
#include <ostream>

namespace Routing {

    struct EdgeRaw {
        int edgeId;
        int nodeIndex;
        int computed_speed;
        int length;
        unsigned short dataIndex;

        EdgeRaw() {};

        EdgeRaw(int edgeId, int nodeIndex, int computed_speed, int length, unsigned short dataIndex) :
                edgeId(edgeId),
                nodeIndex(nodeIndex),
                computed_speed(computed_speed),
                length(length),
                dataIndex(dataIndex) {};

        friend std::ostream &operator<<(std::ostream &os, const EdgeRaw &edgeRaw) {
            os << "E: " << edgeRaw.edgeId << ", N: " << edgeRaw.nodeIndex << ", L: " <<
               edgeRaw.length << ", DI: " << edgeRaw.dataIndex;

            return os;
        };
    };
}


