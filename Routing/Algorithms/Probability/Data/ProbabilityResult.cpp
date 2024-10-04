#include "ProbabilityResult.h"
#include "../Helpers/Utility.h"
#include <cmath>

Routing::Probability::ProbabilityResult::ProbabilityResult(std::map<float, int> times, int computingTime,
                                                           int optimalTravelTime, int sampleCount) {
    this->times = times;
    this->computingTime = computingTime;
    this->optimalTravelTime = optimalTravelTime;
    this->sampleCount = sampleCount;

    this->mean = 0.0;
    this->sampleDev = 0.0;

    this->minTime = this->times.begin()->first;
    this->maxTime = this->times.rbegin()->first;

    this->minP = 0.0f;
    this->maxP = 0.0f;

    // Onepass algorithm based on Mark Hoemmenn, "Computing the standard deviation efficiently", 2007.
    // http://suave_skola.varak.net/proj2/stddev.pdf

    double M = times.begin()->first;
    double Q = 0.0;

    float prev = 0.0;
    long total = 0;
    for (const auto &t : times) {
        for (size_t i = 0; i < static_cast<size_t>(t.second); ++i) {
            Q += (total * std::pow(t.first - M, 2)) / (total + 1);
            M += (t.first - M) / (total + 1);
            total++;
        }

        this->cdf[t.first] = prev = (t.second / static_cast<float>(sampleCount)) + prev;
    }

    this->mean = M;
    if (times.size() > 1) {
        this->sampleDev = std::sqrt(Q / (sampleCount - 1));
    } else {
        this->sampleDev = 0.0;
    }
}


double Routing::Probability::ProbabilityResult::GetMeanAbsoluteError(std::map<float, float> &dist1,
                                                                     std::map<float, float> &dist2) {
    std::map<int, float> dist1Seconds;
    Utility::AggregateSeconds(dist1, dist1Seconds);

    std::map<int, float> dist2Seconds;
    Utility::AggregateSeconds(dist2, dist2Seconds);

    int minIndex = std::min(dist1Seconds.begin()->first, dist2Seconds.begin()->first);
    int maxIndex = std::max(dist1Seconds.rbegin()->first, dist2Seconds.rbegin()->first);

    double MAE = 0;
    for (int i = minIndex; i <= maxIndex; ++i) {
        double dist1Probability = 0;
        auto dist1Find = dist1Seconds.find(i);
        if (dist1Find != dist1Seconds.end()) dist1Probability = dist1Find->second;

        double dist2Probability = 0;
        auto dist2Find = dist2Seconds.find(i);
        if (dist2Find != dist2Seconds.end()) dist2Probability = dist2Find->second;

        MAE += std::abs(dist1Probability - dist2Probability);
    }

    MAE = MAE / (maxIndex - minIndex);

    return MAE;
}

double Routing::Probability::ProbabilityResult::Sum(std::map<float, float> &source) {
    double sum = 0;

    for (const auto &p: source) {
        sum += (double) p.second;
    }

    return sum;
}

float Routing::Probability::ProbabilityResult::ProbabilityForTravelTime(int travelTime) const {
    // Obtain nearest probabilities
    auto lower = this->cdf.lower_bound(travelTime);
    auto upper = this->cdf.upper_bound(travelTime);

    // If lower and upper differs (travelTime exists in map)
    // or we are significantly far from beginning, return lower bound
    if (lower != upper || lower == this->cdf.begin()) {
        return lower->second;
    }

    // Move lower to the nearest lower item
    --lower;

    // Interpolate discrete probabilities
    float x1 = upper->first;
    float y1 = upper->second;
    float x0 = lower->first;
    float y0 = lower->second;
    return (y0 * (x1 - travelTime) + y1 * (travelTime - x0)) / (x1 - x0);
}

std::vector<std::pair<int, float>>
Routing::Probability::ProbabilityResult::GetDelayProbability(std::vector<float> percentiles) const {
    float range = this->maxTime - this->minTime;
    std::vector<std::pair<int, float>> result;

    for (const auto &pct : percentiles) {
        int delay = (int) std::round(range * pct);
        result.emplace_back(delay, 1.0f - this->ProbabilityForTravelTime(this->optimalTravelTime + delay));
    }
    return result;
}


