#pragma once

#include <string>
#include <vector>
#include <memory>
#include "SpeedProbability.h"
#include "ProbabilityProfile.h"

namespace Routing {

    namespace Probability {

        struct Segment {

            long id;
            int length;
            int maxSpeed;
            std::shared_ptr<ProbabilityProfile> profile;

        public:
            Segment(long id, int length, int maxSpeed, std::shared_ptr<ProbabilityProfile> profile) :
                    id(id), length(length), maxSpeed(maxSpeed), profile(profile) {}
        };
    }
}


