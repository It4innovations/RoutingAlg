#pragma once

#include <ostream>

namespace Routing {

    struct ShortcutPosition {
    public:
        int edgeId;
        int position;
        int count;

        ShortcutPosition() {};

        ShortcutPosition(int edgeId, int position, int count) :
                edgeId(edgeId),
                position(position),
                count(count) {};

        std::ostream &operator<<(std::ostream &os) {
            os << "ID: " << this->edgeId << ", Position: " << this->position << ", Count: " << this->count;
            return os;
        }
    };
}


