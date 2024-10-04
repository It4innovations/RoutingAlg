
#pragma once
#include <gtest/gtest.h>
#include <exception>
#include <memory>
#include "../Routing/Data/Probability/ProfileStorageHDF5.h"
#include "DataPaths.h"
#include "Environment.h"

namespace
{

	class ProfileStorageTest : public ::testing::Test {

		protected:
			virtual void SetUp() {
				this->profileStorage = GRAPH_ENV->profileStorage;
			}

			shared_ptr<Routing::Data::Probability::ProfileStorageHDF5> profileStorage;

			void CheckProfile(std::shared_ptr<Routing::Probability::ProbabilityProfile> profile) {

				EXPECT_EQ(profile->GetIntervalDuration(), 900);
				EXPECT_EQ(profile->GetProfilesDuration(), 672 * 900);
				EXPECT_EQ(profile->GetProfiles().size(), 672);

				for(const auto& p : profile->GetProfiles()) {
					double sum = 0.0;
					for(const auto& sp : p) {
						sum += sp.probability;
						EXPECT_TRUE(sp.velocity > 0.0f && sp.velocity < 250.0f);
					}

					for (int it = 0; it < 672; ++it) // Intervals
					{
						for (int idx = 0; idx < 4; ++idx) // Indexes (LoS)
						{
							float aprob = profile->GetAliasProbability(it, idx);
							EXPECT_TRUE(aprob >= 0.0f && aprob <= 1.0f);

							int aidx = profile->GetAliasIndex(it, idx);
							EXPECT_TRUE(aidx >= -1 && aidx <= 3);
						}
					}

					//EXPECT_FLOAT_EQ(sum, 1.0f); // TODO: Fix the rounding dammit
					EXPECT_NEAR(sum, 1.0f, 0.002f);
				}
			}

			void CheckSpeedProfile(std::shared_ptr<std::vector<float>> profile)
			{
				EXPECT_EQ(profile->size(), 672);
				for(const auto& speed : *profile)
				{
					EXPECT_TRUE(speed > 0.0f && speed <= 250.0f);
				}
			}

			void CheckStorage(Routing::Data::Probability::ProfileStorageBase& storage) {
				EXPECT_EQ(storage.GetIntervalCount(), 672);
				EXPECT_EQ(storage.GetIntervalLength(), 900);
				EXPECT_EQ(storage.GetProfileCount(), 4);
				EXPECT_EQ(storage.GetSegmentCount(), 200);
			}

	};

	TEST_F(ProfileStorageTest, LoadFromHDF5File) {

		auto storage = this->profileStorage;
		this->CheckStorage(*storage);
		for (const auto &id : storage->ListSegments()) {
			this->CheckProfile(storage->GetProbabilityProfileByEdgeId(id));
		}
	}

	TEST_F(ProfileStorageTest, GetSpeedProfileByEdgeId) {

		auto storage = this->profileStorage;
		for (const auto &id : storage->ListSegments()) {
			this->CheckSpeedProfile(storage->GetSpeedProfileByEdgeId(id));
		}
	}

	TEST_F(ProfileStorageTest, HDF5ProbabilityNotFound) {

        auto storage = this->profileStorage;
		auto profile = storage->GetProbabilityProfileByEdgeId(9999); // Non existent id
		EXPECT_EQ(profile, nullptr);
	}

	TEST_F(ProfileStorageTest, HDF5SpeedNotFound) {

        auto storage = this->profileStorage;
		auto profile = storage->GetSpeedProfileByEdgeId(9999); // Non existent id
		EXPECT_EQ(profile, nullptr);
	}

	TEST_F(ProfileStorageTest, LoadFromHDF5Memory) {

        auto storage = this->profileStorage;
		this->CheckStorage(*storage);
		for (const auto &id : storage->ListSegments()) {
			this->CheckProfile(storage->GetProbabilityProfileByEdgeId(id));
			this->CheckSpeedProfile(storage->GetSpeedProfileByEdgeId(id));
		}
	}
}