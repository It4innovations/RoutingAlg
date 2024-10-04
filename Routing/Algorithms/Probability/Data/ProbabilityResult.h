#pragma once

#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <map>

namespace Routing {

    namespace Probability {

        class ProbabilityResult {

        public:
            ProbabilityResult(std::map<float, int> times, int computingTime, int optimalTravelTime,
                              int sampleCount);

            static double Sum(std::map<float, float> &source);

            static double GetMeanAbsoluteError(std::map<float, float> &dist1, std::map<float, float> &dist2);

            std::vector<std::pair<int, float>>
            GetDelayProbability(std::vector<float> percentiles = {0.05, 0.25, 0.35, 0.50, 0.65, 0.75, 0.95}) const;

            float ProbabilityForTravelTime(int travelTime) const;

            float minTime;
            float maxTime;
            float minP;
            float maxP;
            float mean;
            float sampleDev;
            int computingTime;
            int optimalTravelTime;
            int sampleCount;
            std::map<float, int> times;
            std::map<float, float> cdf;

            friend std::ostream &operator<<(std::ostream &os, const ProbabilityResult &prob) {
                // Print results and statistics
                os << std::setw(8) << std::setprecision(3);
                os << "Time (min/mean/max): " << prob.minTime / 60.0f << "/" << prob.mean / 60.0f << "/"
                   << prob.maxTime / 60.0f << " min" << std::endl;
                os << "Prob (min/max): " << prob.minP << "/" << prob.maxP << std::endl;
                os << "Sample dev: " << prob.sampleDev / 60.0f << " min" << std::endl;
                os << "Computing time: " << prob.computingTime / 60.0f << " min" << std::endl;
                os << "Optimal time: " << prob.optimalTravelTime / 60.0f << " min" << std::endl;
                os << "Samples: " << prob.sampleCount << std::endl;
                os << "Delay (min) \tProbability" << std::endl;

                for (auto &dp : prob.GetDelayProbability()) {
                    os << dp.first / 60.0f << "\t\t\t" << dp.second << std::endl;
                }

                return os;
            };
        };
    }
}


