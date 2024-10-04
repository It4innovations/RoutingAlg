#pragma once
#include <gtest/gtest.h>
#include "../Routing/Tests/IOUtils.h"
#include "../Routing/Algorithms/OneToMany/OneToManyDijkstra.h"
#include "DataPaths.h"
#include "Environment.h"

namespace {

	class RoutingTestOneToMany : public ::testing::Test {

	protected:
		virtual void SetUp() {
			this->routingGraph = GRAPH_ENV->routingGraph;

			auto idQueries = Routing::Tests::LoadIdQuerySet(ID_QUERY_SET);
			for (const auto &query: idQueries)
				this->idQuerySet.push_back(query);
		}

		shared_ptr<GraphMemory> routingGraph;
		std::vector<std::tuple<int, int>> idQuerySet;
	};

	TEST_F(RoutingTestOneToMany, OneToManyDijkstra) {
		vector<int> idSet;
		for (unsigned int i = 0; i < this->idQuerySet.size(); ++i)
			idSet.push_back(get<0>(this->idQuerySet[i]));

        OneToManyDijkstra *distanceAlgorithm = new Routing::Algorithms::OneToManyDijkstra(this->routingGraph);
		auto distanceResults = distanceAlgorithm->GetDistances(idSet[0], idSet);
		for (unsigned int i = 0; i < distanceResults->distances->size(); i++) {
			cout << distanceResults->distances->at(i) << endl;
		}
	}

    TEST_F(RoutingTestOneToMany, OneToManyDijkstraVariable) {
        vector<int> idSet;
        for (unsigned int i = 0; i < this->idQuerySet.size(); ++i)
            idSet.push_back(get<0>(this->idQuerySet[i]));

        OneToManyDijkstra *distanceAlgorithm = new Routing::Algorithms::OneToManyDijkstra(this->routingGraph);
        auto distanceResults = distanceAlgorithm->GetDistances(idSet[0], idSet, idSet[2]);
        for (unsigned int i = 0; i < distanceResults->distances->size(); i++) {
            cout << distanceResults->distances->at(i) << endl;
        }
    }
}
