#pragma once
#include <gtest/gtest.h>
#include <exception>
#include <omp.h>
#include <random>
#include <utility>

#include "../Routing/Algorithms/Probability/Algorithms/MonteCarloProbability.h"
#include "../Routing/Algorithms/Probability/Helpers/Utility.h"
#include "../Routing/Algorithms/OneToOne/Dijkstra/Dijkstra.h"
#include "../Routing/Algorithms/Common/Result.h"
#include "../Routing/Data/Probability/ProfileStorageHDF5.h"
#include "../Routing/Algorithms/Probability/Data/Data.h"
#include "../Routing/Tests/IOUtils.h"
#include "DataPaths.h"
#include "Environment.h"

namespace {

	class ProbabilityRoutingTest : public ::testing::Test {

	protected:
		virtual void SetUp() {

			this->profileStorage = GRAPH_ENV->profileStorage;
			this->routingGraph = GRAPH_ENV->routingGraph;
			auto idQueries = Routing::Tests::LoadIdQuerySet(ID_QUERY_SET);
			Routing::Algorithms::Dijkstra routingAlg(this->routingGraph);

			// Route query
			Routing::Node n1 = this->routingGraph->GetNodeById(std::get<0>(idQueries[0]));
			Routing::Node n2 = this->routingGraph->GetNodeById(std::get<1>(idQueries[1]));

#ifndef NDEBUG
            cout << endl << "From: " << n1 << endl << "To: " << n2 << endl;
#endif

			//
			auto result = routingAlg.GetResult(n1.id, n2.id, CostCalcType::Len_cost);
			long defaultSegmentId = profileStorage->ListSegments()[0];

			ASSERT_GE(result->GetResult().size(), 0); // Expect paths size > 0

			// Convert routing segments to probability segments
			int travelledDistance = 0;
			for(const Routing::Algorithms::Segment &seg : result->GetResult()) {

				try {
					// Use first segment in storage, we might not have profiles for all segments yet
					if(profileStorage->HasSegment(seg.edgeId)) {
						defaultSegmentId = seg.edgeId;
					}
					int actualSegmentLength = seg.length - travelledDistance;
					travelledDistance = seg.length;
					probabilitySegments.emplace_back(seg.edgeId, actualSegmentLength, seg.length / seg.time,
													 profileStorage->GetProbabilityProfileByEdgeId(defaultSegmentId));
				} catch (Routing::Exception::ProfileNotFound e) {
					ADD_FAILURE() << e.what();
				}
			}
		}

		std::vector<Routing::Probability::Segment> probabilitySegments;
		std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5> profileStorage;
		std::shared_ptr<GraphMemory> routingGraph;
		std::vector<std::unique_ptr<Routing::Algorithms::Result>> paths;

		std::vector<int> departureTime = {0, 8, 2, 1}; // Day, Hour, Minute, Second
	};

	TEST_F(ProbabilityRoutingTest, WeekSeconds) {

		EXPECT_EQ(Routing::Probability::Utility::GetWeekSeconds(0, 0, 0, 0), 0);
		EXPECT_EQ(Routing::Probability::Utility::GetWeekSeconds(16, 12, 0, 0), 216000);
		EXPECT_EQ(Routing::Probability::Utility::GetWeekSeconds(2, 12, 0, 0), 216000);
	}

	TEST_F(ProbabilityRoutingTest, MCProbabilityDebugSeed) {

		// Run only on single thread
		int maxThreads = omp_get_max_threads();
		omp_set_num_threads(1);

#ifndef NDEBUG
		std::cout << "Running instance A" << std::endl;
#endif
		Routing::Probability::MonteCarloProbability mcp(100, this->profileStorage->GetIntervalCount() * this->profileStorage->GetIntervalLength(), 1);
		auto resultA = mcp.GetProbabilityResult(this->probabilitySegments, (Routing::Probability::Utility::GetWeekSeconds(departureTime[0],departureTime[1],departureTime[2],departureTime[3])));

		EXPECT_GT(resultA.computingTime, 0);
		EXPECT_GT(resultA.optimalTravelTime, 0);
		EXPECT_GT(resultA.times.size(), 0);
		EXPECT_LE(resultA.times.size(), 100);

#ifndef NDEBUG
        std::cout << "Running instance B" << std::endl;
#endif
		auto resultB = mcp.GetProbabilityResult(this->probabilitySegments, (Routing::Probability::Utility::GetWeekSeconds(departureTime[0],departureTime[1],departureTime[2],departureTime[3])));

		EXPECT_GT(resultB.computingTime, 0);
		EXPECT_GT(resultB.optimalTravelTime, 0);
		EXPECT_GT(resultB.times.size(), 0);
		EXPECT_LE(resultB.times.size(), 100);

#ifndef NDEBUG
        std::cout << "Comparing" << std::endl;
#endif
		auto iterA = resultA.times.begin();
		auto iterB = resultB.times.begin();
		for (size_t i = 0; i < 99; ++i) {
			ASSERT_NE(iterA, resultA.times.end());
			ASSERT_NE(iterB, resultB.times.end());

			EXPECT_EQ(iterA->first, iterB->first);
			EXPECT_EQ(iterA->second, iterB->second);

			iterA++;
			iterB++;
		}

		omp_set_num_threads(maxThreads);
	}

	TEST_F(ProbabilityRoutingTest, NoProfilesAvail)
	{
		// Run on single thread
		omp_set_num_threads(1);

		auto emptyProbSegments = this->probabilitySegments;
		for (auto &seg : emptyProbSegments)
		{
			seg.profile = nullptr;
		}

		Routing::Probability::MonteCarloProbability mcp(100, this->profileStorage->GetIntervalCount() * this->profileStorage->GetIntervalLength(), 1);
		auto result = mcp.GetProbabilityResult(emptyProbSegments, (Routing::Probability::Utility::GetWeekSeconds(departureTime[0],departureTime[1],departureTime[2],departureTime[3])));

		EXPECT_GT(result.computingTime, 0);
		EXPECT_GT(result.optimalTravelTime, 0);
		EXPECT_FLOAT_EQ(result.sampleDev, 0);
		EXPECT_GT(result.times.size(), 0);
		EXPECT_LE(result.times.size(), 100);
	}

	TEST_F(ProbabilityRoutingTest, MixProfilesAvail)
	{
		// Run on single thread
		omp_set_num_threads(1);

		int existingSegmentIdx = 0;
		auto existingSegments = this->profileStorage->ListSegments();

		auto emptyProbSegments = this->probabilitySegments;
		for (auto &seg : emptyProbSegments)
		{

			if(existingSegmentIdx % 2)
			{
				seg.profile = this->profileStorage->GetProbabilityProfileByEdgeId(
						existingSegments[existingSegmentIdx]);
			}
			else
			{
				seg.profile = nullptr;
			}

			existingSegmentIdx++;

			if((size_t)existingSegmentIdx >= existingSegments.size())
			{
				existingSegmentIdx = 0;
			}
		}

		Routing::Probability::MonteCarloProbability mcp(100, this->profileStorage->GetIntervalCount() * this->profileStorage->GetIntervalLength(), 1);
		auto result = mcp.GetProbabilityResult(emptyProbSegments, (Routing::Probability::Utility::GetWeekSeconds(departureTime[0],departureTime[1],departureTime[2],departureTime[3])));

		EXPECT_GT(result.computingTime, 0);
		EXPECT_GT(result.optimalTravelTime, 0);
		EXPECT_GT(result.sampleDev, 0);
		EXPECT_GT(result.times.size(), 0);
		EXPECT_LE(result.times.size(), 100);
	}

	TEST_F(ProbabilityRoutingTest, AllProfilesAvail)
	{
		// Run on single thread
		omp_set_num_threads(1);

		int existingSegmentIdx = 0;
		auto existingSegments = this->profileStorage->ListSegments();

		auto emptyProbSegments = this->probabilitySegments;
		for (auto &seg : emptyProbSegments)
		{

			seg.profile = this->profileStorage->GetProbabilityProfileByEdgeId(
						existingSegments[existingSegmentIdx++]);

			if((size_t)existingSegmentIdx >= existingSegments.size())
			{
				existingSegmentIdx = 0;
			}
		}

		Routing::Probability::MonteCarloProbability mcp(100, this->profileStorage->GetIntervalCount() * this->profileStorage->GetIntervalLength(), 1);
		auto result = mcp.GetProbabilityResult(emptyProbSegments, (Routing::Probability::Utility::GetWeekSeconds(departureTime[0],departureTime[1],departureTime[2],departureTime[3])));

		EXPECT_GT(result.computingTime, 0);
		EXPECT_GT(result.optimalTravelTime, 0);
		EXPECT_GT(result.sampleDev, 0);
		EXPECT_GT(result.times.size(), 0);
		EXPECT_LE(result.times.size(), 100);
	}

	TEST_F(ProbabilityRoutingTest, InvalidDeparture)
	{
		// Run on single thread
		omp_set_num_threads(1);

		int existingSegmentIdx = 0;
		auto existingSegments = this->profileStorage->ListSegments();

		auto emptyProbSegments = this->probabilitySegments;
		for (auto &seg : emptyProbSegments)
		{

			seg.profile = this->profileStorage->GetProbabilityProfileByEdgeId(
						existingSegments[existingSegmentIdx++]);

			if((size_t)existingSegmentIdx >= existingSegments.size())
			{
				existingSegmentIdx = 0;
			}
		}

		Routing::Probability::MonteCarloProbability mcp(100, this->profileStorage->GetIntervalCount() * this->profileStorage->GetIntervalLength(), 1);
		auto result = mcp.GetProbabilityResult(emptyProbSegments, 999999);

		EXPECT_GT(result.computingTime, 0);
		EXPECT_GT(result.optimalTravelTime, 0);
		EXPECT_GT(result.sampleDev, 0);
		EXPECT_GT(result.times.size(), 0);
		EXPECT_LE(result.times.size(), 100);
	}
}
