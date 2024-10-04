
#include "ProbabilityProfile.h"

#include <cmath>
#include <iostream>
#include <stack>

unsigned int Routing::Probability::ProbabilityProfile::GetInterval(float time) const {
    return (unsigned int) (time / intervalDuration);
}

float
Routing::Probability::ProbabilityProfile::GetTravelTimeToNextInterval(unsigned int currentInterval, float time) const {
    return ((currentInterval + 1) * intervalDuration) - time;
}

std::vector<Routing::Probability::SpeedProbability>
Routing::Probability::ProbabilityProfile::GetProfiles(unsigned int interval) const {
    return profiles[interval];
}

unsigned int Routing::Probability::ProbabilityProfile::GetIntervalDuration() const {
    return intervalDuration;
}


void Routing::Probability::ProbabilityProfile::BuildAliasTable() {
    /**
     * This method implements Vose's alias tables as described here: http://www.keithschwarz.com/darts-dice-coins/.
     */

    int profileCount = profiles.size();
    int LosCount = profiles.front().size();

    // Initialize member fields for alias table
    alias.resize(LosCount * profileCount);
    probability.resize(LosCount * profileCount);
    for (size_t j = 0; j < alias.size(); ++j) {
        alias[j] = 0;
        probability[j] = 0.0f;
    }

    // Intermediate
    std::vector<float> scaled(LosCount);
    std::stack<int> small;
    std::stack<int> large;

    // Iterate over intervals
    for (int s = 0; s < profileCount; ++s) {
        int idx = s * LosCount;

        // Build large and small stacks
        for (int i = 0; i < LosCount; ++i) {

            scaled[i] = LosCount * profiles[s][i].probability;

            if (scaled[i] < 1.0f) {
                small.push(i);
            } else {
                large.push(i);
            }
        }

        // Build alias and prob tables
        while (!small.empty() && !large.empty()) {
            int sm = small.top();
            small.pop();
            int lg = large.top();
            large.pop();

            probability[idx + sm] = scaled[sm];
            alias[idx + sm] = lg;

            scaled[lg] = (scaled[lg] + scaled[sm]) - 1.0f;

            if (scaled[lg] < 1.0f) {
                small.push(lg);
            } else {
                large.push(lg);
            }
        }

        while (!small.empty()) {
            probability[idx + small.top()] = 1.0f;
            small.pop();
        }

        while (!large.empty()) {
            probability[idx + large.top()] = 1.0f;
            large.pop();
        }
    }
}

unsigned int Routing::Probability::ProbabilityProfile::GetProfilesDuration() const {
    return profilesDuration;
}

Routing::Probability::ProbabilityProfile::ProbabilityProfile(const std::vector<std::vector<SpeedProbability>> profiles,
                                                             const std::vector<int> alias,
                                                             const std::vector<float> probability,
                                                             unsigned int intervalLength)
        : profiles(profiles), alias(alias), probability(probability), intervalDuration(intervalLength) {
    profilesDuration = static_cast<unsigned int>(profiles.size() * intervalLength);
    LoSCount = profiles.front().size();
    //BuildAliasTable();
}


float Routing::Probability::ProbabilityProfile::GetNormalizedTime(float time) {
    int overlapCount = static_cast<int>(time / profilesDuration);
    return time - overlapCount * profilesDuration;
}

std::vector<std::vector<Routing::Probability::SpeedProbability>>
Routing::Probability::ProbabilityProfile::GetProfiles() const {
    return this->profiles;
}

float Routing::Probability::ProbabilityProfile::GetAliasProbability(const int interval, const int index) const {
    return probability[(interval * LoSCount) + index];
}

int Routing::Probability::ProbabilityProfile::GetAliasIndex(const int interval, const int index) const {
    return alias[(interval * LoSCount) + index];
}
