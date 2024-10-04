#pragma once

#include <cstring>
#include <map>

#include "Segment.h"
#include "SpeedProbability.h"
#include "TimeProbability.h"
#include "ProbabilityProfile.h"

namespace Routing {

    namespace Probability {

        class Data {

        public:
            static std::vector<Segment> LoadSegments(const std::string &segmentsFile,
                                                     const std::string &profilesDir);

            static void WriteResultTravelTime(const std::map<float, float> &result, const std::string &&file,
                                              float startSeconds = 0);

            static ProbabilityProfile
            GenerateProfile(const std::vector<float> &probDist, const std::vector<float> &speedDist,
                            const float optimalSpeed, const unsigned int profileCount, const unsigned int profLength,
                            const unsigned int roadClass);


            static unsigned int LoadSpeedProfile(const std::string &speedProfileFile,
                                                 std::vector<std::vector<SpeedProbability>> &profileData);
        };
    }
}


