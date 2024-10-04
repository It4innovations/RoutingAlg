#pragma once

#define DIJKSTRA 0
#define BIDIRECTION_DIJKSTRA 1
#define ASTAR 2
#define BIDIRECTION_ASTAR 3

#define ALTERNATIVES_PLATEAU_ALGORITHM 0

#include <gtest/gtest.h>
#include "../Routing/Tests/IOUtils.h"
#include "../Routing/Algorithms/Base/StaticRoutingAlgorithm.h"
#include "../Routing/Algorithms/OneToOne/Dijkstra/Dijkstra.h"
#include "../Routing/Algorithms/OneToOne/Dijkstra/BidirectionalDijkstra.h"
#include "../Routing/Algorithms/OneToOne/Astar/Astar.h"
#include "../Routing/Algorithms/OneToOne/Astar/BidirectionalAstar.h"
#include "../Routing/Algorithms/GraphFilter/GraphFilterGeometry.h"
#include "../Routing/Algorithms/TDAlgorithms/TDDijkstra.h"
#include "../Routing/Algorithms/TDAlgorithms/TDUtility.h"
#include "../Routing/Data/Indexes/GraphMemory.h"
#include "DataPaths.h"
#include "Environment.h"

namespace {

	class RoutingTestOneToOne : public ::testing::Test {

	protected:
		virtual void SetUp() {
			this->routingGraph = GRAPH_ENV->routingGraph;

			auto idQueries = Routing::Tests::LoadIdQuerySet(ID_QUERY_SET);

            this->algs.push_back(new Routing::Algorithms::Dijkstra(this->routingGraph));
            this->algs.push_back(new Routing::Algorithms::BidirectionalDijkstra(this->routingGraph));
            this->algs.push_back(new Routing::Algorithms::Astar(this->routingGraph));
            this->algs.push_back(new Routing::Algorithms::BidirectionalAstar(this->routingGraph));

			for (const auto &query: idQueries) {
				this->idQuerySet.push_back(query);
			}
		}

		shared_ptr<GraphMemory> routingGraph;
		std::vector<std::tuple<int, int>> idQuerySet;
	    std::vector<Routing::Algorithms::StaticRoutingAlgorithm *> algs;


        void RunAlg(Routing::Algorithms::StaticRoutingAlgorithm * ptrAlg){
            for (std::tuple<int, int> &query: this->idQuerySet) {
#ifndef NDEBUG
                cout << "Test: " << get<0>(query) << " " << get<1>(query) << endl;
#endif

                Routing::Node n1 = this->routingGraph->GetNodeById(get<0>(query));
                Routing::Node n2 = this->routingGraph->GetNodeById(get<1>(query));

#ifndef NDEBUG
                cout << endl << "From: " << n1 << endl << "To: " << n2 << endl;
#endif

                unique_ptr<Result> result = ptrAlg[0].GetResult(n1.id, n2.id, CostCalcType::Len_cost);

                auto testResult = GetResultFromFile(ptrAlg, n1.id, n2.id);

                //No path found by algorithm and no obtained result (right test)
                if(testResult.empty() && result == nullptr){
                    continue;
                }

                //No path found by algorithm but obtained result (test failed)
                if(!testResult.empty() && result == nullptr){
                    ADD_FAILURE() << "Algorithm did not find the path (empty ptr).";
                    return;
                }

                CheckResult(testResult, *result);
            }
        }

        std::vector<int> GetResultFromFile(Routing::Algorithms::StaticRoutingAlgorithm * ptrAlg, int nodeIdFrom, int nodeIdTo){

            std::vector<int> testResult;

            if(this->algs[DIJKSTRA] == ptrAlg){
                testResult = Routing::Tests::LoadTestResult(std::string(QUERY_SET_RESULTS_DIR) + std::string("/") + to_string(nodeIdFrom)
                                                        + std::string("_") + to_string(nodeIdTo) + std::string("_Dijkstra.csv"));

            }else if(this->algs[BIDIRECTION_DIJKSTRA] == ptrAlg){
                testResult = Routing::Tests::LoadTestResult(std::string(QUERY_SET_RESULTS_DIR) + std::string("/") + to_string(nodeIdFrom)
                                                        + std::string("_") + to_string(nodeIdTo) + std::string("_BidirectionalDijkstra.csv"));

            }else if(this->algs[ASTAR] == ptrAlg){
                testResult = Routing::Tests::LoadTestResult(std::string(QUERY_SET_RESULTS_DIR) + std::string("/") + to_string(nodeIdFrom)
                                                        + std::string("_") + to_string(nodeIdTo) + std::string("_Astar.csv"));

            }else if(this->algs[BIDIRECTION_ASTAR] == ptrAlg){
                testResult = Routing::Tests::LoadTestResult(std::string(QUERY_SET_RESULTS_DIR) + std::string("/") + to_string(nodeIdFrom)
                                                        + std::string("_") + to_string(nodeIdTo) + std::string("_BidirectionalAstar.csv"));

            } else{
                std::cout<< "No implementation of this Alg" << endl;
                exit(-1);
            }

            return testResult;
        }

        void CheckResult(const std::vector<int>& testResult, Routing::Algorithms::Result& result){

            if(testResult.empty()){
                //Path found by algorithm and no obtained result (test failed)
                ADD_FAILURE() << "The file of result(" << result.GetResult()[0].nodeId1 << " " << result.GetResult()[0].nodeId2 << ") does not exist.";
                return;
            }

            //control of result and expected result (right test)
            for (unsigned int i = 0; i < result.GetResult().size(); i++) {
                if(testResult.at(i) != result.GetResult()[i].edgeId){
                    ADD_FAILURE() << "Bad path: Segment number " << i << ". EdgeID expected: " << testResult.at(i) <<
                                  " EdgeID obtained:" << result.GetResult()[i].edgeId;
                }
            }
        }
	};


	TEST_F(RoutingTestOneToOne, Dijkstra) {

        RunAlg(this->algs[DIJKSTRA]);

	}

	TEST_F(RoutingTestOneToOne, BidirectionalDijkstra) {

        RunAlg(this->algs[BIDIRECTION_DIJKSTRA]);

	}

	TEST_F(RoutingTestOneToOne, Astar) {

        RunAlg(this->algs[ASTAR]);

	}

	TEST_F(RoutingTestOneToOne, BidirectionalAstar) {

        RunAlg(this->algs[BIDIRECTION_ASTAR]);

	}
}

