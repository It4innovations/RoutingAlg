#pragma once

#include <vector>
#include "SpeedProbability.h"

namespace Routing {

    namespace Probability {

        class ProbabilityProfile {

        public:
            ProbabilityProfile(const std::vector<std::vector<SpeedProbability>> profiles, const std::vector<int> alias,
                               const std::vector<float> probability, unsigned int intervalLength);

            std::vector<SpeedProbability> GetProfiles(unsigned int interval) const;

            unsigned int GetInterval(float time) const;

            float GetTravelTimeToNextInterval(unsigned int currentInterval, float time) const;

            // Seconds from the beginning of the profile
            float GetNormalizedTime(float time);

            unsigned int GetIntervalDuration() const;

            unsigned int GetProfilesDuration() const;

            void BuildAliasTable();

            std::vector<std::vector<SpeedProbability>> GetProfiles() const;

            float GetAliasProbability(const int interval, const int index) const;

            int GetAliasIndex(const int interval, const int index) const;

            std::vector<std::vector<SpeedProbability>> profiles;

        private:
            std::vector<int> alias;
            std::vector<float> probability;

            unsigned int intervalDuration; // seconds
            unsigned int profilesDuration; // seconds
            unsigned int LoSCount;
        };
    }
}


