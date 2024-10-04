#pragma once
#include <gtest/gtest.h>
#include <exception>
#include "DataPaths.h"
#include "../Routing/Tests/IOUtils.h"
#include "../Routing/Pipeline/RoutingPipeline.h"
#include "../Routing/Pipeline/RoutingRequest.h"
#include "../Routing/Pipeline/PipelineConfig.h"
#include "../Routing/Data/Results/PipelineResultStorageSQLite.h"

using namespace Routing::Pipeline;

namespace
{
	class RoutingPipelineTest : public ::testing::Test
	{
		protected:
			virtual void SetUp()
			{

			}

			PipelineConfig GetConfig()
			{
				return PipelineConfig(PIPELINE_CONFIG);
			}

			std::vector<RoutingRequest> GetRequests()
			{
				return RoutingRequest::LoadRequests(ID_QUERY_SET, false);
			}
	};

	TEST_F(RoutingPipelineTest, ConfigLoading) {
		PipelineConfig pipelineConfig = GetConfig();

		EXPECT_EQ(pipelineConfig.routingAlgorithm, PipelineConfig::RoutingAlgorithm::Dijkstra);
		EXPECT_EQ(pipelineConfig.alternativeCount, 10);
		EXPECT_EQ(pipelineConfig.ptdr, false);
		EXPECT_EQ(pipelineConfig.samples, 1000);
		EXPECT_EQ(pipelineConfig.reorderingType, PipelineConfig::ReorderingType::Average);
	}

	TEST_F(RoutingPipelineTest, MissingConfFile) {
		EXPECT_ANY_THROW(PipelineConfig("dummy_missing_file.conf"));
	}

	TEST_F(RoutingPipelineTest, OneToOneRouting) {

		PipelineConfig pipelineConfig = GetConfig();
		pipelineConfig.routingAlgorithm = PipelineConfig::RoutingAlgorithm::Dijkstra;
		pipelineConfig.ptdr = false;

		RoutingPipeline pipeline(pipelineConfig);

		for(auto &request : GetRequests())
		{
			pipeline.ProcessRequest(request);
			EXPECT_GE(request.routes.back().size(), 0);
		}
	}

	TEST_F(RoutingPipelineTest, OneToOnePTDRRouting) {
		PipelineConfig pipelineConfig = GetConfig();
		pipelineConfig.routingAlgorithm = PipelineConfig::RoutingAlgorithm::Dijkstra;
		pipelineConfig.ptdr = true;
		pipelineConfig.samples = 100;

		RoutingPipeline pipeline(pipelineConfig);

		for(auto &request : GetRequests())
		{
			pipeline.ProcessRequest(request);
			EXPECT_EQ(request.routes.size(), 1);
			EXPECT_EQ(request.probabilityResult.size(), 1);
		}
	}

	TEST_F(RoutingPipelineTest, OneToOneNodeNotFound) {
		PipelineConfig pipelineConfig = GetConfig();
		pipelineConfig.routingAlgorithm = PipelineConfig::RoutingAlgorithm::Dijkstra;
		pipelineConfig.ptdr = false;
		pipelineConfig.samples = 100;

		RoutingPipeline pipeline(pipelineConfig);

		RoutingRequest request(1, 0000001, 999999);

		EXPECT_ANY_THROW(pipeline.ProcessRequest(request));
	}

	TEST_F(RoutingPipelineTest, ResultStorage) {
		PipelineConfig pipelineConfig = GetConfig();
		pipelineConfig.writeResultsSQLite = true;
		pipelineConfig.writeResultsCSV = true;

		RoutingPipeline pipeline(pipelineConfig);
		PipelineResultStorageSQLite storage(pipelineConfig, std::to_string(time(nullptr)) + "-result.sql");

		auto requests = GetRequests();
		for(auto &request : requests)
		{
			pipeline.ProcessRequest(request);
			EXPECT_EQ(request.routes.size(), 1);
			EXPECT_EQ(request.probabilityResult.size(), 0);
			storage.StoreResult(request);
		}
	}
}
