#pragma once

namespace Routing {

    namespace Algorithms {

        class HeuristicTravelCostCalculator {

        public:
            virtual float GetTravelCost(int length, float time, unsigned char funcClass, float distance) const {
                return time + distance;
            }

            static HeuristicTravelCostCalculator &Instance() {
                static HeuristicTravelCostCalculator singleton;
                return singleton;
            }
        };
    }
}


