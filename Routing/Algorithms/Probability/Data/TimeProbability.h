#pragma once

#include <ostream>

namespace Routing {

    namespace Probability {

        struct TimeProbability {

            float time;
            float probability;

            TimeProbability() : time(0), probability(0) {}

            TimeProbability(float time, float probability) : time(time), probability(probability) {}

            friend std::ostream &operator<<(std::ostream &os, const TimeProbability &tp) {
                os << "T: " << tp.time << ", P: " << tp.probability;
                return os;
            }
        };
    }
}

