#pragma once

#include <iostream>
#include <memory>

#include "../Data/ProbabilityResult.h"
#include "../../../Data/Utility/TimeWatch.h"
#include "../Data/Segment.h"

namespace Routing {

    namespace Probability {

        class ProbabilityAlgorithm {

        public:
            virtual std::map<float, int>
            GetProbability(const std::vector<Segment> &segments, float startSeconds) = 0;

            float GetOptimalTravelTime(const std::vector<Routing::Probability::Segment> &segments, float startSeconds);

            ProbabilityResult GetProbabilityResult(const std::vector<Segment> &segments, float startSeconds);

            virtual ~ProbabilityAlgorithm() {};

        protected:
            unsigned int sampleCount;
            int defaultProfileInterval; // Time interval for speed and probability profiles (seconds)
        };
    }
}


