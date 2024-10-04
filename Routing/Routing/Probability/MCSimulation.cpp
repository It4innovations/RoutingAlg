#include "MCSimulation.h"
#include <mkl_vsl.h>
#include <omp.h>
#include <random>
#include <mkl.h>
#include <iostream>
#include <fstream>
#include "CSVReader.h"
#include <dirent.h>
#include <map>
#include <cstring>

#define ALIGN 64 // Align memory to this byte boundary
#define RANDS_PER_SEGMENT 5 // Margin of random numbers per segment generated
#define SEED 1 // RNG Seed 
#define PROFILE_FILE_NAME_SEP "_" // Separator in file names
#define INDEX_RESOLUTION 10 // Size of the speed profile array

std::vector<float>
Routing::MCSimulation::RunMonteCarloSimulation(int samples, int startDay, int startHour, int startMinute,
                                               bool single) const {
    int intervalsPerDay = 86400 / m_secondInterval;
    std::vector<float> travelTimes;
#pragma omp parallel shared(travelTimes)
    {
        int tid = omp_get_thread_num();
        VSLStreamStatePtr rndStream;
        vslNewStream(&rndStream, VSL_BRNG_MT2203 + tid, SEED);
        int probsSize = m_segmentCount * RANDS_PER_SEGMENT;
        int *probs = (int *) mkl_malloc(sizeof(int) * probsSize, ALIGN);

        if (!single) {
#pragma omp single
            {
                travelTimes.resize(intervalsPerDay * 7 * samples);
            }

#pragma omp for schedule(dynamic)
            for (int s = 0; s < samples; ++s) {
                for (int d = 0; d < 7; ++d) {
                    for (int i = 0; i < intervalsPerDay; ++i) {
                        viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rndStream, probsSize, probs, 0, INDEX_RESOLUTION);
                        int secs = (d * 86400) + (i * 900);
                        travelTimes[(d * intervalsPerDay * samples) + (i * samples) +
                                    s] = Routing::MCSimulation::GetRandomTravelTime(secs, probs);
                    }
                }
            }
        } else {
#pragma omp single
            {
                travelTimes.resize(samples);
            }

#pragma omp for schedule(dynamic)
            for (int s = 0; s < samples; ++s) {
                viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rndStream, probsSize, probs, 0, INDEX_RESOLUTION);
                int secs = (startDay * 86400) + (startHour * 3600) + (startMinute * 60);
                travelTimes[s] = Routing::MCSimulation::GetRandomTravelTime(secs, probs);
            }
        }
        mkl_free(probs);
    }
    return travelTimes;
}

float Routing::MCSimulation::GetRandomTravelTime(int startSeconds, int *probs) const {
    int pIdx = 0;
    float totalTravelTime = 0;
    float currentSeconds = (float) startSeconds;
    for (int s = 0; s < m_segmentCount; ++s) {
        float remainingLength = m_lengths[s];
        while (remainingLength > 0) {
            int currentInterval = currentSeconds / m_secondInterval;
            int idx = (currentInterval * INDEX_RESOLUTION) + probs[pIdx++];
            float velocity = m_speedProfiles[s][idx];
            float currentTravelTime = remainingLength / velocity; // Rounded to seconds
            float newSeconds = (currentSeconds + currentTravelTime);
            int newInterval = newSeconds / m_secondInterval;

            // Increase total travel time
            totalTravelTime += currentTravelTime;

            // Check if travel time was within single time interval (i.e. single speed profile)
            // Suggest via builtin_expect that this condition will be false in most cases (based on the data)
            if (__builtin_expect(newInterval != currentInterval, 0)) {
                // If not, compute distance travelled in time remaining to next interval
                int secsToNext =
                        ((currentSeconds / m_secondInterval) * m_secondInterval + m_secondInterval) - currentSeconds;
                remainingLength -= (velocity * secsToNext);
                currentTravelTime = secsToNext;

                // Resolve wrapping of the week
                if (newSeconds < 604800) {
                    // Travel time is still within one week
                    currentSeconds += currentTravelTime;
                } else {
                    // We may have wrapped around one or more weeks... (better not)
                    int weekOverlap = newSeconds / 604800; // How many weeks are within this overlap
                    currentSeconds = newSeconds - (weekOverlap *
                                                   604800); // Set remainder as new current secs from the beginning of the week
                }
            } else {
                remainingLength = 0; // Go to next segment
            }
        }
    }
    return totalTravelTime;
}

void Routing::MCSimulation::LoadSpeedProfile(const std::string speedProfileFile, float **speedProfileData) {
    const float oneDiv3point6 = 1 / 3.6; // For conversion of km/h to m/s
    std::ifstream profileFileStream(speedProfileFile);
    if (!profileFileStream.is_open()) {
        std::cerr << "ERROR: Cannot open profile file " << speedProfileFile << std::endl;
    }

    CSVReader row('|');

    // Get intervals from first two rows of the file
    std::vector<int> hours(2);
    std::vector<int> min(2);

    int cnt = 0;
    while (profileFileStream >> row && cnt < 2) {
        hours[cnt] = std::stoi(row[1]);
        min[cnt] = std::stoi(row[2]);
        cnt++;
    }

    if (hours[0] == hours[1]) {
        // Interval is less than 1 hour
        m_secondInterval = 60 * (min[1] - min[0]);
    } else {
        // Interval is longer than 1 hour
        m_secondInterval = (hours[1] - hours[0]) * 3600;
    }

    // Get profile count from number of columns in file
    int profilesPerInterval =
            (row.size() - 3) / 2; // Skip first three columns, divide by two values in single SpeedProbability
    int intervalsPerDay = 86400 / m_secondInterval;

    //profileData.resize(INDEX_RESOLUTION * 7 * intervalsPerDay);
    *speedProfileData = (float *) mkl_malloc(INDEX_RESOLUTION * 7 * intervalsPerDay * sizeof(float), ALIGN);

    // Rewind stream
    profileFileStream.seekg(0);

    while (profileFileStream >> row) {
        // Day of week
        int currentDay = -1;
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
        int currentProfileIdx =
                (((hour * 3600) + (min * 60)) / (int) m_secondInterval) + (currentDay * intervalsPerDay);

        int startIdx = 0;
        for (int i = 0; i < profilesPerInterval; i++) {

            int rowIdx = i * 2 + 3; // Skip first three columns

            float velocity = std::stof(row[rowIdx]) * oneDiv3point6; // Convert velocity from km/h to m/s
            float probability = std::stof(row[rowIdx + 1]);

            int length = (probability == 1.0) ? INDEX_RESOLUTION - 1 : (int) round(INDEX_RESOLUTION * probability);

            if (length == 0)
                continue;

            for (int j = startIdx; j < (startIdx + length) + 1; ++j) {
                int index = (currentProfileIdx * INDEX_RESOLUTION) + j;
                (*speedProfileData)[index] = velocity;
            }
            startIdx += length;
        }
    }
    profileFileStream.close();
}

void Routing::MCSimulation::LoadSegments(const std::string segmentsFile, const std::string profilesDir) {

    // Load files in profile directory
    DIR *dirp = opendir(profilesDir.c_str());
    struct dirent *entry;
    std::map<std::string, std::string> profilesByTmcId;

    if (!dirp) {
        std::cerr << "ERROR: Cannot open directory " << profilesDir << std::endl;
    }

    char *fileName = new char[1024];
    while ((entry = readdir(dirp))) {
        if (entry->d_type == DT_REG) {

            char *token = strtok(strncpy(fileName, entry->d_name, 1024), PROFILE_FILE_NAME_SEP);
            // UK
            profilesByTmcId.emplace(std::string(token), profilesDir + '/' + std::string(entry->d_name));
        }
    }

    delete[] fileName;
    m_segmentCount = profilesByTmcId.size();
    if (m_segmentCount < 1)
        std::cerr << "ERROR: No segments found in directory " << profilesDir << std::endl;

    // Load segments
    std::ifstream segmentFileStream(segmentsFile);
    // Allocate memory for lengths
    m_lengths = (int *) mkl_malloc(m_segmentCount * sizeof(int), ALIGN);
    // Allocate memory for speed profiles
    m_speedProfiles = (float **) mkl_malloc(m_segmentCount * sizeof(float *), ALIGN);

    if (!segmentFileStream.is_open()) {
        std::cerr << "ERROR: Unable to open file " << segmentsFile << std::endl;
        std::exit(EXIT_FAILURE);
    } else {
        CSVReader row(';');
        int cnt = 0;
        segmentFileStream >> row; // Discard the header
        while (segmentFileStream >> row) {
            if (row.size() != 3) {
                std::cerr << "ERROR: Row " << cnt + 1 << " has invalid column count." << std::endl;
                continue;
            }
            std::string tmcId = row[0];
            if (profilesByTmcId.find(tmcId) == profilesByTmcId.end()) {
                std::cerr << "ERROR: Profile for segment " << tmcId << " not found in profile directory " << profilesDir
                          << std::endl;
                continue;
            }
            LoadSpeedProfile(profilesByTmcId[tmcId], &m_speedProfiles[cnt]);
            m_lengths[cnt] = std::stoi(row[1]);
            cnt++;
        }
        segmentFileStream.close();
    }
}

Routing::MCSimulation::~MCSimulation() {
    if (m_lengths != nullptr)
        mkl_free(m_lengths);

    if (m_speedProfiles != nullptr) {
        for (int i = 0; i < m_segmentCount; ++i) {
            mkl_free(m_speedProfiles[i]);
        }
        mkl_free(m_speedProfiles);
    }

}

Routing::MCSimulation::MCSimulation(const std::string segmentsFile, const std::string profilesDir) {
    LoadSegments(segmentsFile, profilesDir);
}

float Routing::MCSimulation::GetSecondInterval() const {
    return m_secondInterval;
}