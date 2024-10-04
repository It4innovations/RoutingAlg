#pragma once

#include <gtest/gtest.h>
#include "DataPaths.h"
#include "../Routing/Data/Indexes/GraphMemory.h"
#include "../Routing/Data/Probability/ProfileStorageHDF5.h"
#include "../Routing/Tests/IOUtils.h"

class RoutingTestEnvironment : public ::testing::Environment {
public:
	void SetUp() override {
		this->routingGraph = Routing::Tests::LoadGraph(HDF_INDEX_PATH);
        this->profileStorage = std::make_shared<Routing::Data::Probability::ProfileStorageHDF5>(HDF_PROFILE_PATH, false);
	}

    shared_ptr<Routing::Data::Probability::ProfileStorageHDF5> profileStorage;
	shared_ptr<Routing::Data::GraphMemory> routingGraph;
};

extern RoutingTestEnvironment* GRAPH_ENV;
