
#include <map>
#include <sys/stat.h>
#include <set>
#include "Data.h"
#include "CSVReader.h"
#include "dirent.h"
#include <cmath>
#include <random>

#define PROFILE_FILE_NAME_SEP "_"


std::vector<Routing::Probability::Segment> Routing::Probability::Data::LoadSegments(const std::string &segmentsFile,
                                                                                    const std::string &profilesDir) {


    // Load files in profile directory
    DIR *dirp = opendir(profilesDir.c_str());
    struct dirent *entry;
    std::map<std::string, std::string> profilesByTmcId;

    if (!dirp) {
        std::cerr << "ERROR: Cannot open directory " << profilesDir << std::endl;
    }

    struct stat s;
    char *fileName = new char[1024];
    while ((entry = readdir(dirp))) {
        stat((profilesDir
        +"//" + std::string(entry->d_name).c_str()).c_str(), &s);
        if (S_ISREG(s.st_mode)) {

            char *token = strtok(strncpy(fileName, entry->d_name, 1024), PROFILE_FILE_NAME_SEP);

            profilesByTmcId.emplace(std::string(token), profilesDir + "//" + std::string(entry->d_name));
        }
    }

    delete[] fileName;

    // Load segments
    std::ifstream segmentFileStream(segmentsFile);
    std::vector<Segment> segmentList;
    if (!segmentFileStream.is_open()) {
        std::cerr << "ERROR: Unable to open file " << segmentsFile << std::endl;
        std::exit(EXIT_FAILURE);
    } else {
        CSVReader row(';');
        int cnt = 0;

        segmentFileStream >> row; // Discard the header

        while (segmentFileStream >> row) {
            cnt++;
            if (row.size() != 3) {
                std::cerr << "ERROR: Row " << cnt + 1 << " has invalid column count." << std::endl;
                continue;
            }

            std::string tmcId = row[0];

            if (profilesByTmcId.find(tmcId) == profilesByTmcId.end()) {
                std::cerr << "ERROR: Profile for segment " << tmcId << " not found in profile directory " <<
                          profilesDir << std::endl;
                continue;
            }

            std::vector<std::vector<SpeedProbability>> speedProfileData;
            unsigned int secondInterval = LoadSpeedProfile(profilesByTmcId[tmcId], speedProfileData);

            long length = std::stol(row[1]);
            double maxSpeed = std::stol(row[2]);

            segmentList.emplace_back(-1, // id
                                     length, // length
                                     maxSpeed, // maxSpeed
                                     ProbabilityProfile(speedProfileData, secondInterval) // Speed profile
            );
        }
        segmentFileStream.close();
    }
    return segmentList;
}

unsigned int Routing::Probability::Data::LoadSpeedProfile(const std::string &speedProfileFile,
                                                          std::vector<std::vector<SpeedProbability>> &profileData) {

    const float oneDiv3point6 = 1 / (float) 3.6; // For conversion of km/h to m/s

    std::ifstream profileFileStream(speedProfileFile);

    if (!profileFileStream.is_open()) {
        std::cerr << "ERROR: Cannot open profile file " << speedProfileFile << std::endl;
    }

    CSVReader row('|');

    // Get intervals from first two rows of the file
    std::vector<unsigned int> hours(2);
    std::vector<unsigned int> min(2);

    int cnt = 0;
    while (profileFileStream >> row && cnt < 2) {
        hours[cnt] = std::stoul(row[1]);
        min[cnt] = std::stoul(row[2]);
        cnt++;
    }

    unsigned int secondInterval = 0;
    if (hours[0] == hours[1]) {
        // Interval is less than 1 hour
        secondInterval = 60 * (min[1] - min[0]);
    } else {
        // Interval is longer than 1 hour
        secondInterval = (hours[1] - hours[0]) * 3600;
    }

    // Get profile count from number of columns in file
    // Skip first three columns, divide by two values in single SpeedProbability
    unsigned int profilesPerInterval = static_cast<unsigned int>((row.size() - 3) / 2);
    unsigned int intervalsPerDay = 86400 / secondInterval;

    profileData.reserve(7 * intervalsPerDay);


    // Rewind stream
    profileFileStream.seekg(0);

    while (profileFileStream >> row) {
        // Day of week
        int currentDay = -1;
        //      int currentDay = std::stoi(row[0]); // Speed profiles in CSV with day as a number
        std::string currentDayString = row[0];


        if (currentDayString == "Monday") currentDay = 0; // Monday
        else if (currentDayString == "Tuesday") currentDay = 1; // Tuesday
        else if (currentDayString == "Wednesday") currentDay = 2; // Wednesday
        else if (currentDayString == "Thursday") currentDay = 3; //
        else if (currentDayString == "Friday") currentDay = 4;
        else if (currentDayString == "Saturday") currentDay = 5;
        else if (currentDayString == "Sunday") currentDay = 6;

        if (currentDay == -1)
            std::cerr << "ERROR: Unknown day of week string " << currentDayString << std::endl;

        // Time interval
        int hour = std::stoi(row[1]);
        int min = std::stoi(row[2]);
        unsigned int currentProfileIdx =
                (((hour * 3600) + (min * 60)) / secondInterval) + (currentDay * intervalsPerDay);

        std::vector<SpeedProbability> speedProbabilities;
        float probabilitySum = 0.0f;

        for (unsigned int i = 0; i < profilesPerInterval; i++) {

            int columnIdx = i * 2 + 3; // Skip first three columns

            float velocity = std::stof(row[columnIdx]) * oneDiv3point6; // Convert velocity from km/h to m/s
            if (velocity <= 0)
                std::cerr << "ERROR: Wrong velocity value " << velocity << std::endl;

            float probability = std::stof(row[columnIdx + 1]);
            if (probability < 0.0f || probability > 1.0f)
                std::cerr << "ERROR: Wrong probability value " << probability << std::endl;

            speedProbabilities.emplace_back(SpeedProbability(velocity, probability));

            probabilitySum += probability;
        }

        if (std::fabs(1.0f - probabilitySum) > std::numeric_limits<float>::epsilon()) {
            std::cerr << "ERROR: Wrong probability sum " << probabilitySum << std::endl;
        }

        if (currentProfileIdx != profileData.size())
            std::cerr << "ERROR: Wrong index of profile " << currentProfileIdx << std::endl;

        profileData.emplace_back(speedProbabilities);
    }

    profileFileStream.close();

    if (profileData.size() != intervalsPerDay * 7)
        std::cerr << "ERROR: Wrong number of intervals in " << speedProfileFile << std::endl;

    return secondInterval;
}

void Routing::Probability::Data::WriteResultTravelTime(const std::map<float, float> &result, const std::string &&file,
                                                       float startSeconds) {

    std::ofstream rfile(file);

    for (auto r: result) {
        if (r.first != 0)
            rfile << r.first - startSeconds << "\t" << r.second << "\n";
    }

    rfile.flush();
    rfile.close();
}

Routing::Probability::ProbabilityProfile
Routing::Probability::Data::GenerateProfile(const std::vector<float> &probDist, const std::vector<float> &speedDist,
                                            const float optimalSpeed, const unsigned int profileCount,
                                            const unsigned int profLength, const unsigned int roadClass) {
    std::vector<std::vector<Routing::Probability::SpeedProbability>> profiles;

    // By default, the profiles are generated for one week
    const unsigned int profilesPerWeek = std::round(7 * 24 * 3600 / profLength);
    const unsigned int profilesPerWorkDays = std::round(5 * 24 * 3600 / profLength);
    profiles.resize(profilesPerWeek);

    std::random_device rd;
    std::mt19937 rng(rd());

    for (unsigned int c = 0; c < profilesPerWeek; c++) {
        std::vector<Routing::Probability::SpeedProbability> singleProfile;
        for (unsigned int i = 0; i < profileCount; i++) {
            Routing::Probability::SpeedProbability prof(optimalSpeed, probDist[i]);

            if (c > profilesPerWorkDays) {
                prof.velocity *= 1.2;
            }

            // Reduce speed by 30% during the night
            if (c % 96 < 24 || c % 96 > 80) {
                prof.velocity *= 0.7;

                // Reduce probability of accident
                if (i >= 1) {
                    prof.probability *= 0.7;
                    //probDist_m[i] = 0.7 * probDist_m[i];
                }
            }
            singleProfile.push_back(prof);
        }

        if (roadClass > 3) {
            for (unsigned int i = 1; i < profileCount; i++) {
                singleProfile[i].probability *= 0.3;
            }
        }

        for (unsigned int i = 0; i < profileCount; i++) {
            float mi = -0.2 * singleProfile[i].probability;
            float ma = 0.2 * singleProfile[i].probability;
            std::uniform_real_distribution<double> uni(mi, ma);
            singleProfile[i].probability += uni(rng);

            if (singleProfile[i].probability < 0) {
                singleProfile[i].probability = 0;
            }
        }

        if (singleProfile[0].probability > 1) {
            singleProfile[0].probability = 1;

            for (unsigned int g = 1; g < profileCount; g++) {
                singleProfile[g].probability = 0;
            }

        } else {
            float total = 0;
            for (unsigned int i = 0; i < profileCount; i++) {
                total += singleProfile[i].probability;
            }
            for (unsigned int i = 0; i < profileCount; i++) {
                singleProfile[i].probability /= total;
            }
        }

        for (size_t p = 0; p < profileCount; p++) {
            singleProfile[p].velocity *= speedDist[p];
#ifdef NDEBUG
            std::cout << c << '\t' << p << '\t' << singleProfile[p].velocity << '\t' << singleProfile[p].probability << std::endl;
#endif
        }
        profiles[c] = singleProfile;
    }
    return Routing::Probability::ProbabilityProfile(profiles, profLength);
}
