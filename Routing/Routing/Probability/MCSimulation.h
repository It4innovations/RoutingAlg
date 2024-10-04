#pragma once

#include <list>
#include <vector>
#include <string>

namespace Routing {
    class MCSimulation {

    public:
        MCSimulation(const std::string segmentsFile, const std::string profilesDir);

        void LoadSegments(const std::string segmentsFile, const std::string profilesDir);

        std::vector<float>
        RunMonteCarloSimulation(const int samples, const int startDay, const int startHour, const int startMinute,
                                bool single) const;

        ~MCSimulation();

        float GetSecondInterval() const;

    private:
        void LoadSpeedProfile(const std::string speedProfileFile, float **speedProfileData);

        float GetRandomTravelTime(int startSeconds, int *probs) const;

        // Members
        float m_secondInterval = 0;
        int m_segmentCount = 0;
        float **m_speedProfiles = nullptr;
        int *m_lengths = nullptr;
    };
}


