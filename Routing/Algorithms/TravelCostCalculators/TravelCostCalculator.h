#pragma once

#include "../../Data/Utility/Definitions.h"

//using Routing::byte;

namespace Routing {

    namespace Algorithms {

        class TravelCostCalculator {

        public:
            virtual float GetTravelCost(int length, float time, unsigned char funcClass) const {
                return time;
            }

            static TravelCostCalculator &Instance() {
                static TravelCostCalculator singleton;
                return singleton;
            }
        };
    }
}


