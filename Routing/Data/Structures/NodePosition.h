#pragma once

#include <ostream>

namespace Routing {

    struct NodePosition {
        int nodeId;

        char partId[ID_INFO_NAME_SZ];
        int nodeIndex;

        friend std::ostream &operator<<(std::ostream &os, const NodePosition npData) {
            os << "ID: " << npData.nodeId << ", Part: " << npData.partId
               << ", Index: <<" << npData.nodeIndex;

            return os;
        }
    };
}


