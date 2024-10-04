#pragma once

#include "DistanceInfo.h"

namespace Routing {

    namespace Algorithms {

        class DistanceResult {
        public:
            std::unique_ptr<std::vector<DistanceInfo>> distances;
            int computingTime;


            DistanceResult(const std::vector<DistanceInfo> &distances, int computingTime)
                    : distances(new std::vector<DistanceInfo>(distances)),
                      computingTime(computingTime) {}
        };
    }
}


