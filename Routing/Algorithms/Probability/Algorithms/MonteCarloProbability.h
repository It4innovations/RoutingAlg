#pragma once

#include <vector>
#include "../Base/ProbabilityAlgorithm.h"

namespace Routing {

    namespace Probability {

        class MonteCarloProbability : public ProbabilityAlgorithm {

        public:
            MonteCarloProbability(unsigned int sampleCount, int defaultProfileInterval, int seed = 1) {

                this->sampleCount = sampleCount;
                this->defaultProfileInterval = defaultProfileInterval;
                this->seed = seed;

#ifdef USE_MKL
                std::string prng("MKL VSL_BRNG_MT2203");
#else
                std::string prng("STL std::mt19937_64");
#endif

#ifndef NDEBUG
                std::cout << "Random number generator: " << prng << std::endl;
#endif
            };

            virtual std::map<float, int>
            GetProbability(const std::vector<Segment> &segments, float startSeconds) override final;

        private:
            int seed;

            float GetRandomTravelTime(const std::vector<Routing::Probability::Segment> &segments, float startSeconds,
                                      std::vector<std::vector<float>> &profiles, int indexResolution,
                                      int *randomNumbers);

            float GetRandomTravelTime(const std::vector<Routing::Probability::Segment> &segments, float startSeconds,
                                      unsigned long long *randomNumbers);


        };
    }
}



