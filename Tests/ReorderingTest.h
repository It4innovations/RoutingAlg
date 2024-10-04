#pragma once

#include <gtest/gtest.h>
#include "DataPaths.h"
#include "../Routing/Tests/IOUtils.h"
#include "../Routing/Algorithms/Reordering/ReorderingBase.h"
#include "../Routing/Algorithms/Reordering/SessionReordering.h"
#include "../Routing/Algorithms/Reordering/Data/ReorderInput.h"
#include "Environment.h"

using namespace Routing::Algorithms;

namespace
{
	class ReorderingTest : public ::testing::Test
	{
	protected:
		virtual void SetUp()
		{
			this->routingGraph = GRAPH_ENV->routingGraph;
		}

		shared_ptr<GraphMemory> routingGraph;

	};


	TEST_F(ReorderingTest, SessionInit)
	{
		SessionReordering re(this->routingGraph, {1.0f, 1.0f, 1.0f});
		SUCCEED();
	}

	TEST_F(ReorderingTest, SessionReorder)
	{
		SessionReordering re(this->routingGraph, {1.0f, 1.0f, 1.0f});
		std::vector<ReorderInput> input;

		const long long int currentTmw = 0;
		const long long int tmwSize = 60;

		// First route
		reorder_count::second_type firstTimeLoad1 =  {std::make_pair(0, 1), std::make_pair(0, 0), std::make_pair(0, 0)};
		reorder_count::second_type firstTimeLoad2 =  {std::make_pair(0,0), std::make_pair(0, 0), std::make_pair(0, 0)};
		std::vector<Algorithms::reorder_count> firstRoute = {std::make_pair(Algorithms::Segment(66682778, 10, 20, 100), // Segment 1
													 		 firstTimeLoad1),
													   std::make_pair(Algorithms::Segment(66682779, 10, 20, 100), // Segment 2
													   		firstTimeLoad2)};
		input.emplace_back(std::move(firstRoute), 0, 10, 0.001f, 0.9f, 1.0f, 10.0f, 5.0f, 1.0f);

		// Second route
		reorder_count::second_type secondTimeLoad1 =  {std::make_pair(0,0), std::make_pair(0, 0), std::make_pair(0, 0)};
		reorder_count::second_type secondTimeLoad2 =  {std::make_pair(0,0), std::make_pair(0, 0), std::make_pair(0, 0)};
		std::vector<Algorithms::reorder_count> secondRoute = {std::make_pair(Algorithms::Segment(66682780, 10, 20, 100), // Segment 1
																			 secondTimeLoad1),
															 std::make_pair(Algorithms::Segment(66682780, 10, 20, 100), // Segment 2
																			secondTimeLoad2)};
		input.emplace_back(std::move(secondRoute), 0, 10, 0.001f, 0.9f, 1.0f, 10.0f, 5.0f, 1.0f);

		// Last route returned
		std::vector<long long> lastRoute = {66682780,66682780};
		std::vector<long long int> result = re.Reorder(input, lastRoute, currentTmw, tmwSize);
		EXPECT_EQ(result.size(), 2);
		EXPECT_EQ(result[0], 1);
		EXPECT_EQ(result[1], 0);
	}


	TEST_F(ReorderingTest, RouteQuantize)
	{

		SessionReordering re(this->routingGraph, {1.0f, 1.0f, 1.0f});

		// Missing last segment
		std::vector<long long> route{66677170, 66677163, 66677163, 66677170}; // Lengths: 599, 23, 23, 599


		// Empty result
		// std::vector<long long> route{66677165, 66677163}; // Lengths: 62, 23

		auto quantized = re.QuantizeTravelTime(route, 0, 60);

#ifndef NDEBUG
		for(const auto& s : quantized)
		{
			std::cout << s.first << ":\t\t";

			for(const auto& q : s.second )
			{
				std::cout << q.first << " (" << q.second <<  "), ";
			}
			std::cout << std::endl;
		}
#endif
	}
}
