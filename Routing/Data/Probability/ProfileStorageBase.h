#pragma once

#include <string>
#include <map>
#include <memory>
#include "../../Algorithms/Probability/Data/ProbabilityProfile.h"
#include "../../Algorithms/Probability/Data/SpeedProbability.h"

namespace Routing {

    namespace Data {

        namespace Probability {
            /**
             * ProfileStorageBase
             * Base class for accessing speed profile storage.
             * Profiles can be loaded to memory upon construction.
             *
             * The class exposes two main methods
             * GetSpeedProfileByEdgeId and GetProbabilityProfileByEdgeId.
             * These methods either access storage and return the required object
             * or return it from internal map.
             *
             * Each concrete storage has to implement
             * ListSegments
             * HasSegment
             * LoadProfiles
             * LoadProbabilityProfileFromStorage
             * LoadSpeedProfileFromStorage
             *
             */
            class ProfileStorageBase {
            public:
                /**
                 * ProfileStorageBase
                 * @param path Path to storage object (dir, file, url..)
                 * @param inMemory if true, ALL profiles are loaded to memory - USE WITH CAUTION
                 */
                ProfileStorageBase(const std::string path, const bool inMemory);

                /**
                 * GetProbabilityProfileByEdgeId
                 * @param edgeId
                 * @return ProbabilityProfile
                 */
                std::shared_ptr<Routing::Probability::ProbabilityProfile> GetProbabilityProfileByEdgeId(
                        const long edgeId);

                /**
                 * GetSpeedProfileByEdgeId
                 * @param edgeId
                 * @return std::vector<int>
                 */
                std::shared_ptr<std::vector<float>> GetSpeedProfileByEdgeId(const long edgeId);

                /**
                 * ListSegments
                 * Provides list of available segments in this storage
                 * @return segmentId
                 */
                virtual std::vector<long> ListSegments() = 0;

                /**
                 * HasSegment
                 * Checks if provided segmentId is present in the storage
                 * @param segmentId
                 * @return
                 */
                virtual bool HasSegment(long segmentId) = 0;

                virtual ~ProfileStorageBase() {};

                // Getters
                unsigned short GetIntervalLength() const;

                unsigned short GetIntervalCount() const;

                unsigned int GetSegmentCount() const;

                unsigned short GetProfileCount() const;

                const std::string &GetVersion() const;

                long GetDefaultSegmentId() const;

            protected:
                virtual void LoadProfiles() = 0;

                virtual std::shared_ptr<Routing::Probability::ProbabilityProfile>
                LoadProbabilityProfileFromStorage(const long edgeId) = 0;

                virtual std::shared_ptr<std::vector<float>> LoadSpeedProfileFromStorage(const long edgeId) = 0;

            protected:

                // Member variables
                const std::string path; // Path to source file (directory)
                const bool inMemory; // Load all profiles to memory
                unsigned short intervalLength; // Length of an interval in seconds
                unsigned short intervalCount; // Number of intervals per segment
                unsigned int segmentCount; // Number of segments available in total
                long defaultSegmentId; // Default segment ID

                // Number of segments in file (directory)
                unsigned short profileCount; // Levels of service (default 4)
                std::string version; // Data version

                // In memory profile storage
                std::map<long, std::shared_ptr<Routing::Probability::ProbabilityProfile>> probabilityProfiles;
                std::map<long, std::shared_ptr<std::vector<float>>> speedProfiles;

                const float oneDiv3point6 = 1 / 3.6f;

            private:

                template<typename T>
                std::shared_ptr<T> LoadFromMemory(const long edgeId, const std::map<long, std::shared_ptr<T>> &storage);

            };
        }
    }
}


