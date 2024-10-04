#pragma once

#include <ostream>

namespace Routing {

    namespace Algorithms {

        class RouteInfo {
        public:
            float Time;
            int Length;

            RouteInfo(int length, float time) :
                    Time(time),
                    Length(length) {};

            friend std::ostream &operator<<(std::ostream &os, const RouteInfo &routeInfo) {
                os << "Time: " << routeInfo.Time << ", Length: " << routeInfo.Length;

                return os;
            }
        };
    }
}

