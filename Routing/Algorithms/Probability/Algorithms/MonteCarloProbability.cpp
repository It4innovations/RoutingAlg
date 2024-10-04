
#include <omp.h>
#include <limits>

#include "MonteCarloProbability.h"
#include "../../../Data/Utility/Definitions.h"

#define ALIGN 64

#ifdef USE_MKL
#include <mkl.h>
#else

#include <random>

#endif

std::map<float, int>
Routing::Probability::MonteCarloProbability::GetProbability(const std::vector<Routing::Probability::Segment> &segments,
                                                            float startSeconds) {

    std::map<float, int> result;

    const unsigned int randomsPerSegment = 100;

    std::vector<float> travelTimes;

#pragma omp parallel shared(travelTimes)
    {

#ifdef INTEL_RND
        VSLStreamStatePtr rndStream;
        vslNewStream(&rndStream, VSL_BRNG_MT2203 + tid, this->seed);
#else
        std::mt19937_64 rnd((unsigned long long) (this->seed));
#endif

        unsigned long long int probsSize = segments.size() * randomsPerSegment;
        unsigned long long *probs = (unsigned long long *) malloc(sizeof(unsigned long long) * probsSize);

        if (probs == nullptr) {
            std::cerr << "Cannot allocate memory for probs." << std::endl;
            throw std::exception();
        }

#pragma omp single
        {
            travelTimes.resize(sampleCount);
        }

#pragma omp for schedule(dynamic)
        for (unsigned int s = 0; s < sampleCount; ++s) {
#ifdef INTEL_RND
            //viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rndStream, probsSize, probs, 0, indexResolution);
            viRngUniformBits64(VSL_RNG_METHOD_UNIFORMBITS64_STD, rndStream, probsSize, probs);
#else
            std::uniform_int_distribution<unsigned long long> dist(0, std::numeric_limits<unsigned long long>::max());
            for (size_t r = 0; r < probsSize; ++r) {
                probs[r] = dist(rnd);
            }
#endif

            travelTimes[s] = GetRandomTravelTime(segments, startSeconds, probs);
        }
        free(probs);
    }

    for (float t: travelTimes) {
        auto timeFind = result.find(t);
        if (timeFind == result.end()) {
            result.insert({t, 1});
        } else {
            timeFind->second += 1;
        }
    }

    return result;
}

float
Routing::Probability::MonteCarloProbability::GetRandomTravelTime(
        const std::vector<Routing::Probability::Segment> &segments, float startSeconds,
        unsigned long long *randomNumbers) {

    int probabilityIndex = 0;
    float totalTravelTime = 0;
    float currentSeconds = startSeconds;

    for (unsigned int s = 0; s < segments.size(); ++s) {
        float remainingLength = segments[s].length;

        if (segments[s].profile == nullptr) {
            // This segment does not have profile, use its max speed
            float travelTime = remainingLength / segments[s].maxSpeed;
            totalTravelTime += travelTime;
            currentSeconds += travelTime;

            // Handle wrapping behind the interval length
            if (unlikely(currentSeconds >= this->defaultProfileInterval)) {
                currentSeconds = (int) currentSeconds % this->defaultProfileInterval;
            }

            continue;
        }

        while (remainingLength > 0) {
            const unsigned int currentInterval = segments[s].profile->GetInterval(currentSeconds);

            /** Alias table sampling **/
            int probIdx = randomNumbers[probabilityIndex++] & 3; // 4 LoS, only first 3 bits are needed
            double rand = randomNumbers[probabilityIndex++] / (double) std::numeric_limits<unsigned long long>::max();

            // Determine alias and prob
            float aliasProb = segments[s].profile->GetAliasProbability(currentInterval, probIdx);
            int aliasIdx;

            if (aliasProb <= rand) {
                aliasIdx = segments[s].profile->GetAliasIndex(currentInterval, probIdx);
            } else {
                aliasIdx = probIdx;
            }

//			if(aliasIdx >= csize) {
//            	std::cerr << "Invalid aliasIdx " << aliasIdx << std::endl;
//				for (int j = 0; j < 4; ++j)
//				{
//					std::cout << segments[s].profile->GetAliasIndex(currentInterval, j) << " -- " << segments[s].profile->GetAliasProbability(currentInterval, j) << std::endl;
//				}
//		    }

            const float velocity = segments[s].profile->profiles[currentInterval][aliasIdx].velocity;
            /** ********************* **/

            const float travelTime = remainingLength / velocity;
            const float newSeconds = currentSeconds + travelTime;
            const unsigned int newInterval = segments[s].profile->GetInterval(newSeconds);

            // Check if travel time was within single time interval (i.e. single speed profile)
            // Suggest via builtin_expect that this condition will be false in most cases (based on the data)
            if (unlikely(newInterval != currentInterval)) {
                // If not, compute distance travelled in time remaining to next interval
                const float travelTimeToIntervalSwitch = segments[s].profile->GetTravelTimeToNextInterval(
                        currentInterval,
                        currentSeconds);
                remainingLength -= (velocity * travelTimeToIntervalSwitch);
                totalTravelTime += travelTimeToIntervalSwitch;

                // Resolve wrapping of the week
                if (unlikely(newSeconds >= segments[s].profile->GetProfilesDuration())) {
                    currentSeconds = 0; // TODO: Remainder of the overlap?
                } else {
                    currentSeconds = currentSeconds + travelTimeToIntervalSwitch;
                }
            } else {
                remainingLength = 0; // Go to next segment
                totalTravelTime += travelTime;
                currentSeconds = newSeconds;
            }
        }
    }
    return totalTravelTime;
}

