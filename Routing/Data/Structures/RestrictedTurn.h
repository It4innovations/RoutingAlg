#pragma once

#include <ostream>

namespace Routing {

    struct RestrictedTurn {
        int fromNodeId;
        int viaNodeId;
        int toNodeId;

        RestrictedTurn() {};

        RestrictedTurn(int fromNodeId, int viaNodeId, int toNodeId) :
                fromNodeId(fromNodeId),
                viaNodeId(viaNodeId),
                toNodeId(toNodeId) {};

        std::ostream &operator<<(std::ostream &os) {
            os << "From: " << this->fromNodeId << ", Via: " << this->viaNodeId
               << ", To: " << this->toNodeId;

            return os;
        }

        bool operator==(const RestrictedTurn &other) {
            return fromNodeId == other.fromNodeId && viaNodeId == other.viaNodeId && toNodeId == other.toNodeId;
        }
    };
}


