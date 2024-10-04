
#include "Utility.h"

#include <set>
#include <chrono>
#include <sstream>
#include <cmath>
#include "../Data/TimeProbability.h"

unsigned int Routing::Probability::Utility::GetWeekSeconds(unsigned int day, unsigned int hour, unsigned int minute,
                                                           unsigned int second) {
    return ((day * 86400) + (hour * 3600) + (minute * 60) + second) % (86400 * 7);
}

void Routing::Probability::Utility::AddResults(std::map<float, float> &destination, std::map<float, float> &source) {

    for (const auto &s: source) {
        auto timeFind = destination.find(s.first);
        if (timeFind == destination.end()) {
            destination.insert(s);
        } else {
            timeFind->second += s.second;
        }
    }
}

void Routing::Probability::Utility::AddResults(std::map<float, float> &destination,
                                               std::unordered_map<float, float> &source) {

    for (const auto &s: source) {
        auto timeFind = destination.find(s.first);
        if (timeFind == destination.end()) {
            destination.insert(s);
        } else {
            timeFind->second += s.second;
        }
    }
}

void Routing::Probability::Utility::Aggregate2(std::map<float, float> &source, std::map<float, float> &result) {

    unsigned int count = 0;
    auto it = source.begin();

    for (; it != source.end() && count + 2 <= source.size(); it++) {
        count = count + 2;

        auto it1 = it;
        auto it2 = ++it;

        //cout << "F: " << it1->first << " S: " << it2->first << endl;
        float p = (it1->second + it2->second);
        float t = (it1->first * it1->second + it2->first * it2->second) / (it1->second + it2->second);

        auto timeFind = result.find(t);
        if (timeFind == result.end()) {
            result.insert({t, p});
        } else {
            timeFind->second += p;
        }
    }

    for (; it != source.end(); it++) {
        //cout << "S: " << it->first << endl;
        auto timeFind = result.find(it->first);
        if (timeFind == result.end()) {
            result.insert({it->first, it->second});
        } else {
            timeFind->second += it->second;
        }
    }
}

void Routing::Probability::Utility::AggregateClosest2(std::map<float, float> &source, std::map<float, float> &result) {

    std::vector<TimeProbability> timeProbabilities;

    for (const auto &t: source) {
        timeProbabilities.push_back(TimeProbability(t.first, t.second));
    }

    std::multimap<float, std::pair<int, int>> distancePositions;

    for (unsigned int i = 0; i + 1 < timeProbabilities.size(); ++i) {

        float distance = std::abs(timeProbabilities[i].time - timeProbabilities[i + 1].time);
        distancePositions.insert({distance, {i, i + 1}});
    }

    std::set<int> usedIndexes;

    for (auto it = distancePositions.begin(); it != distancePositions.end(); ++it) {

        if (usedIndexes.find(it->second.first) != usedIndexes.end()) continue;
        if (usedIndexes.find(it->second.second) != usedIndexes.end()) continue;

        const TimeProbability &t1 = timeProbabilities[it->second.first];
        const TimeProbability &t2 = timeProbabilities[it->second.second];

        //cout << "F: " << it1->first << " S: " << it2->first << endl;
        const float p = (t1.probability + t2.probability);
        const float t = (t1.time * t1.probability + t2.time * t2.probability) / (t1.probability + t2.probability);

        auto timeFind = result.find(t);
        if (timeFind == result.end()) {
            result.insert({t, p});
        } else {
            timeFind->second += p;
        }

        usedIndexes.insert(it->second.first);
        usedIndexes.insert(it->second.second);
    }

    for (unsigned int i = 0; i < timeProbabilities.size(); ++i) {
        if (usedIndexes.find(i) == usedIndexes.end()) {
            const TimeProbability &t = timeProbabilities[i];
            auto timeFind = result.find(t.time);
            if (timeFind == result.end()) {
                result.insert({t.time, t.probability});
            } else {
                timeFind->second += t.probability;
            }
        }
    }
}

void Routing::Probability::Utility::Aggregate3(std::map<float, float> &source, std::map<float, float> &result) {

    unsigned int count = 0;
    auto it = source.begin();

    for (; it != source.end() && count + 3 <= source.size(); it++) {
        count += 3;

        auto it1 = it;
        auto it2 = ++it;
        auto it3 = ++it;

        //cout << "F: " << it1->first << " S: " << it2->first << endl;
        const float l = (it2->first - it3->first) / (it1->first - it3->first);
        const float p1 = it1->second + l * it2->second;
        const float p3 = it3->second + (1 - l) * it2->second;

        auto time1Find = result.find(it1->first);
        if (time1Find == result.end()) {
            result.insert({it1->first, p1});
        } else {
            time1Find->second += p1;
        }

        auto time3Find = result.find(it3->first);
        if (time3Find == result.end()) {
            result.insert({it3->first, p3});
        } else {
            time3Find->second += p3;
        }
    }

    //if (count < source.size()) it++;

    for (; it != source.end(); it++) {
        //cout << "S: " << it->first << endl;
        auto timeFind = result.find(it->first);
        if (timeFind == result.end()) {
            result.insert({it->first, it->second});
        } else {
            timeFind->second += it->second;
        }
    }
}

void Routing::Probability::Utility::AggregateSeconds(std::map<float, float> &source, std::map<float, float> &result) {

    for (auto it = source.begin(); it != source.end(); it++) {

        const float time = std::nearbyint(it->first);
        auto timeFind = result.find(time);
        if (timeFind == result.end()) {
            result.insert({time, it->second});
        } else {
            timeFind->second += it->second;
        }
    }
}

void Routing::Probability::Utility::AggregateSeconds(std::map<float, float> &source, std::map<int, float> &result) {
    for (const auto &s: source) {
        const int seconds = static_cast<int>(nearbyint(s.first));

        auto sourceFind = result.find(seconds);

        if (sourceFind == result.end()) {
            result.insert({seconds, s.second});
        } else {
            sourceFind->second += s.second;
        }
    }
}

void Routing::Probability::Utility::AggregateSeconds(std::unordered_map<float, float> &source,
                                                     std::unordered_map<float, float> &result) {

    for (auto it = source.begin(); it != source.end(); it++) {

        const float time = std::nearbyint(it->first);
        auto timeFind = result.find(time);
        if (timeFind == result.end()) {
            result.insert({time, it->second});
        } else {
            timeFind->second += it->second;
        }
    }
}

void Routing::Probability::Utility::AggregateSecondsFraction(std::unordered_map<float, float> &source,
                                                             std::unordered_map<float, float> &result,
                                                             float secondFraction) {

    for (auto it = source.begin(); it != source.end(); it++) {

        const float time = std::nearbyint(it->first * secondFraction) / secondFraction;
        auto timeFind = result.find(time);
        if (timeFind == result.end()) {
            result.insert({time, it->second});
        } else {
            timeFind->second += it->second;
        }
    }
}

std::string Routing::Probability::Utility::GetReadableTime(int weekSeconds) {
    std::stringstream output;

    // Day
    std::vector<std::string> days = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    output << days[static_cast<size_t>(weekSeconds / 86400)] << ",";

    // Hour
    int hour = weekSeconds % 86400;
    int hourVal = static_cast<int>(hour / 3600);
    output << ((hourVal < 10) ? "0" : "") << hourVal << ":";

    // Minute
    int min = hour % 3600;
    int minVal = static_cast<int>(min / 60);
    output << ((minVal < 10) ? "0" : "") << minVal << ":";

    // Second
    int sec = min % 60;
    int secVal = static_cast<int>(sec);
    output << ((secVal < 10) ? "0" : "") << secVal;

    return output.str();
}
