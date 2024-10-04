#pragma once

#include "../Common/RouteInfo.h"

using Routing::Algorithms::RouteInfo;

namespace Routing {

    namespace Algorithms {

        class RouteSolution {
        public:
            int IntersectionId;
            Routing::Algorithms::RouteInfo RouteInfo;

            RouteSolution(int intersectionId, Routing::Algorithms::RouteInfo routeInfo);
        };
    }
}


