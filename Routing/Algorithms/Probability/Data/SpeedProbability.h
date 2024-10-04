#pragma once

namespace Routing {

    namespace Probability {

        struct SpeedProbability {

            float velocity;
            float probability;

            SpeedProbability(float velocity, float probability) : velocity(velocity), probability(probability) {}
        };
    }
}

