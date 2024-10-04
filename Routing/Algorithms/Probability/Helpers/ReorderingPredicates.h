#pragma once

#include "../Data/ProbabilityResult.h"

namespace Routing {

    namespace Probability {

        namespace Reordering {

            struct ReorderBase {
            };

            struct MeanTime : ReorderBase {

                bool operator()(ProbabilityResult lhs, ProbabilityResult rhs) const { return lhs.mean < rhs.mean; }
            };

            struct MinTime : ReorderBase {

                bool operator()(ProbabilityResult lhs, ProbabilityResult rhs) const {
                    return lhs.minTime < rhs.minTime;
                }
            };

            struct MaxTime : ReorderBase {

                bool operator()(ProbabilityResult lhs, ProbabilityResult rhs) const {
                    return lhs.maxTime < rhs.maxTime;
                }
            };

            struct StdDev : ReorderBase {

                bool operator()(ProbabilityResult lhs, ProbabilityResult rhs) const {
                    return lhs.sampleDev < rhs.sampleDev;
                }
            };
        }
    }
}



