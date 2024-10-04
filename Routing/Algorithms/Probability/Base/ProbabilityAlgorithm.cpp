
#include "ProbabilityAlgorithm.h"

Routing::Probability::ProbabilityResult
Routing::Probability::ProbabilityAlgorithm::GetProbabilityResult(const std::vector<Segment> &segments,
                                                                 float startSeconds) {

    Routing::Utils::TimeWatch watch(true);

    // Wrap around start seconds in case the value is over the interval
    if (startSeconds >= this->defaultProfileInterval) {
        startSeconds = (int) startSeconds % this->defaultProfileInterval;
        std::cerr << "Warning: Start seconds over profile length - new start time " << startSeconds << " s"
                  << std::endl;
    }

    auto probabilities = this->GetProbability(segments, startSeconds);

    watch.Stop();

    return Routing::Probability::ProbabilityResult(probabilities, watch.GetElapsed(),
                                                   this->GetOptimalTravelTime(segments, startSeconds),
                                                   this->sampleCount);
}


float Routing::Probability::ProbabilityAlgorithm::GetOptimalTravelTime(
        const std::vector<Routing::Probability::Segment> &segments, float startSeconds) {
    // Optimal travel time = speed of 100% Level of Service, first speed profile
    float optimalTravelTime = 0.0f; // seconds
    for (const auto &segment : segments) {

        float speed = 0.0f;

        if (segment.profile != nullptr) {
            speed = segment.profile->profiles[segment.profile->GetInterval(startSeconds)][0].velocity;
        } else {
            speed = segment.maxSpeed;
        }

        optimalTravelTime += segment.length / speed;
    }
    return optimalTravelTime;
}