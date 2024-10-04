#include <gtest/gtest.h>
#include "Environment.h"

#include "GraphAndDataStructTest.h"
#include "ProbabilityRoutingTest.h"
#include "ProfileStorageTest.h"
#include "ReorderingTest.h"
#include "RoutingPipelineTest.h"
#include "RoutingTestOneToOne.h"
#include "RoutingTestOneToMany.h"
#include "RoutingTestTime.h"
#include "RoutingTestAlternatives.h"

RoutingTestEnvironment* GRAPH_ENV;

int main(int argc, char** argv)
{
	GRAPH_ENV = dynamic_cast<RoutingTestEnvironment*>(::testing::AddGlobalTestEnvironment(new RoutingTestEnvironment));

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
