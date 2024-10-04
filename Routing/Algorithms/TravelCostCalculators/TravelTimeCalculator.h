#pragma once


namespace Routing {

    namespace Algorithms {

        class TravelTimeCalculator {

        public:
            virtual float GetTravelTime(int length, float speedMPS) {
                return length / speedMPS;
            }

            static TravelTimeCalculator &Instance() {
                static TravelTimeCalculator singleton;
                return singleton;
            }
        };
    }
}


