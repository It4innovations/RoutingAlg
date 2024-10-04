#pragma once

#include <vector>
#include <memory>
#include <map>
#include <unordered_map>

namespace Routing {

    namespace Probability {

        class Utility {

        public:
            static unsigned int GetWeekSeconds(unsigned int day, unsigned int hour, unsigned int minute,
                                               unsigned int second);

            static std::string GetReadableTime(int weekSeconds);

            static void AddResults(std::map<float, float> &destination, std::map<float, float> &source);

            static void AddResults(std::map<float, float> &destination, std::unordered_map<float, float> &source);

            static void Aggregate2(std::map<float, float> &source, std::map<float, float> &result);

            static void Aggregate3(std::map<float, float> &source, std::map<float, float> &result);

            static void AggregateSeconds(std::map<float, float> &source, std::map<float, float> &result);

            static void AggregateSeconds(std::map<float, float> &source, std::map<int, float> &result);

            static void
            AggregateSeconds(std::unordered_map<float, float> &source, std::unordered_map<float, float> &result);

            static void
            AggregateSecondsFraction(std::unordered_map<float, float> &source, std::unordered_map<float, float> &result,
                                     float secondFraction);

            static void AggregateClosest2(std::map<float, float> &source, std::map<float, float> &result);
        };
    }
}


