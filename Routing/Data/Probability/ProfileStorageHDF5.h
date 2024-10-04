#pragma once

#include <mutex>

#include "ProfileStorageBase.h"
#include <H5Cpp.h>

namespace Routing {

    namespace Data {

        namespace Probability {
            extern "C" herr_t GroupIterator(hid_t loc_id, const char *name, void *opdata);

            class ProfileStorageHDF5 : public ProfileStorageBase {
            public:
                ProfileStorageHDF5(const std::string &path, const bool inMemory);

                std::vector<long> ListSegments() override;

                bool HasSegment(long segmentId) override;

            protected:
                void LoadProfiles() override;

                std::shared_ptr<Routing::Probability::ProbabilityProfile>
                LoadProbabilityProfileFromStorage(const long edgeId) override;

                std::shared_ptr<std::vector<float>> LoadSpeedProfileFromStorage(const long edgeId) override;

            private:
                /**
                 * LoadSpeedProbability
                 * Load 2D vector of SpeedProbability pairs directly from HDF5 dataset
                 * @param dataset
                 * @return 2D vector
                 */
                std::vector<std::vector<Routing::Probability::SpeedProbability>>
                LoadSpeedProbability(const H5::DataSet &dataset);

                /**
                 * LoadFlatDataset
                 * Templated method to load 2D dataset of basic type to vector.
                 * Expect to load dataset with rank = 2, with dimensions this->intervalCount x columns
                 * see below for specializations
                 * @tparam T - type to load
                 * @param dataset
                 * @param columns
                 * @return vector with loaded dataset
                 */
                template<typename T>
                std::vector<T> LoadFlatDataset(const H5::DataSet &dataset, long columns, H5::PredType type);

                // The almighty HDF5 file
                // Structured into groups named by individual segment ids
                H5::H5File h5file;
                std::mutex h5file_mutex;

                // Names of datasets in each group
                const std::string PROBABILITY_DATASET_NAME = "prob_profile";
                const std::string SPEED_DATASET_NAME = "speed_profile";
                const std::string ALIAS_DATASET_NAME = "alias";
                const std::string PROB_ALIAS_DATASET_NAME = "probability";

            };
        }
    }
}
