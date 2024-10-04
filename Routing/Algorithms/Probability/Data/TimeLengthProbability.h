#pragma once

namespace Routing {

    namespace Probability {

        struct TimeLengthProbability {

            float actualTime;
            float travelTime;
            float length;
            float probability;

            TimeLengthProbability() : actualTime(0), travelTime(0), length(0), probability(0) {}

            TimeLengthProbability(float actualTime, float travelTime, float length, float probability)
                    : actualTime(actualTime), travelTime(travelTime), length(length), probability(probability) {}
        };
    }
}


