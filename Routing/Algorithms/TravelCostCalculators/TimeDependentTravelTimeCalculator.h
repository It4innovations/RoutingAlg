#pragma once

#include "../../Data/Utility/Definitions.h"

using Routing::byte;

namespace Routing {

    namespace Algorithms {

        class TimeDependentTravelTimeCalculator {

        public:
            virtual float GetTravelTime(int edgeId, float currentTime, int length, float speedMPS) {
                return length / speedMPS;
            }

            static TimeDependentTravelTimeCalculator &Instance() {
                static TimeDependentTravelTimeCalculator singleton;
                return singleton;
            }
        };
    }
}


