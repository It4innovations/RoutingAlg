#pragma once

#include <vector>
#include "../../Common/Segment.h"

namespace Routing {

    namespace Algorithms {
        //Segment with vector -> each category with its count
        typedef std::pair<Routing::Algorithms::Segment, std::vector<std::pair<long long, long long>>> reorder_count;

        struct ReorderInput {
            ReorderInput(std::vector<reorder_count> &&segments,
                         int computingTime, int optimalTravelTime, float minP, float maxP,
                         float minTime, float maxTime, float mean, float sampleDev) :
                    segments(std::move(segments)),
                    computingTime(computingTime),
                    optimalTravelTime(optimalTravelTime),
                    minP(minP),
                    maxP(maxP),
                    minTime(minTime),
                    maxTime(maxTime),
                    mean(mean),
                    sampleDev(sampleDev) {};

            std::vector<reorder_count> segments;
            int computingTime;
            int optimalTravelTime;
            float minP;
            float maxP;
            float minTime;
            float maxTime;
            float mean;
            float sampleDev;
        };
    }

}

