#pragma once

#include <ostream>

namespace Routing {

    namespace Probability {

        struct SegmentTimeProbability {

            unsigned int number;
            float time;
            float probability;

            SegmentTimeProbability() : number(0), time(0), probability(0) {}

            SegmentTimeProbability(unsigned int number, float time, float probability) : number(number), time(time),
                                                                                         probability(probability) {}

            friend std::ostream &operator<<(std::ostream &os, const SegmentTimeProbability &segment) {
                os << "N: " << segment.number << ", T: " << segment.time << ", P: " << segment.probability;
                return os;
            }
        };
    }
}


