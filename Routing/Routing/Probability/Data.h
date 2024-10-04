#pragma once

#include <list>

namespace Routing {

    class Data {

    public:
        static int LoadSegments(const std::string segmentsFile, const std::string profilesDir, int &secondInterval,
                                float **speedProfiles, int *lengths);

        static void WriteResultSingle(std::vector<float> &result, std::string &&file, float secondInterval);

        static void WriteResultAll(std::vector<float> &result, std::string &&file, int samples, float secondInterval);

    private:
        static int LoadSpeedProfile(const std::string speedProfileFile, std::vector<float> &profileData);
    };
}


