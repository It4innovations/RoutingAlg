#pragma once

#define ALTERNATIVES_PLATEAU_ALGORITHM 0

#include <gtest/gtest.h>
#include "../Routing/Tests/IOUtils.h"
#include "../Routing/Algorithms/Base/StaticRoutingAlgorithm.h"
#include "../Routing/Algorithms/Alternatives/AlternativesPlateauAlgorithm.h"
#include "../Routing/Algorithms/GraphFilter/GraphFilterGeometry.h"
#include "../Routing/Algorithms/TDAlgorithms/TDDAlternativesUtility.h"
#include "../Routing/Data/Indexes/GraphMemory.h"
#include "DataPaths.h"
#include "Environment.h"

namespace {

    class RoutingTestAlternatives : public ::testing::Test {

    protected:
        virtual void SetUp() {
            this->routingGraph = GRAPH_ENV->routingGraph;

            auto idQueries = Routing::Tests::LoadIdQuerySet(ID_QUERY_SET);

            this->alternativesAlgs.push_back(new Routing::Algorithms::AlternativesPlateauAlgorithm(this->routingGraph));


            for (const auto &query: idQueries) {
                this->idQuerySet.push_back(query);
            }
        }

        unsigned int alternativesCount = 2;

        shared_ptr<GraphMemory> routingGraph;
        std::vector<std::tuple<int, int>> idQuerySet;
        std::vector<Routing::Algorithms::AlternativesAlgorithm *> alternativesAlgs;


        void RunAlternativeAlg(Routing::Algorithms::AlternativesAlgorithm * ptrAlg){
            for (std::tuple<int, int> &query: this->idQuerySet) {
#ifndef NDEBUG
                cout << "Test: " << get<0>(query) << " " << get<1>(query) << endl;
#endif

                Routing::Node n1 = this->routingGraph->GetNodeById(get<0>(query));
                Routing::Node n2 = this->routingGraph->GetNodeById(get<1>(query));

#ifndef NDEBUG
                cout << endl << "From: " << n1 << endl << "To: " << n2 << endl;
#endif

                auto results = alternativesAlgs[0]->GetResults(n1.id, n2.id, 3, true);

                auto testResults = GetResultFromFilesAlternatives(ptrAlg, n1.id, n2.id);

                //No path found by algorithm and no obtained result (right test)
                if(testResults.at(0).empty() && results == nullptr){
                    continue;
                }

                //No path found by algorithm but obtained result (test failed)
                if(!testResults.at(0).empty() && results == nullptr){
                    ADD_FAILURE() << "Algorithm did not find the path (empty ptr).";
                    return;
                }

                if(results->size() == 0) {
                    ADD_FAILURE() << "Expected more results.";
                    return;
                }

                CheckResult(testResults, std::move(results));
            }
        }

        //return two expected result
        std::vector<std::vector<int>> GetResultFromFilesAlternatives(Routing::Algorithms::AlternativesAlgorithm * ptrAlg, int nodeIdFrom, int nodeIdTo){

            std::vector<std::vector<int>> testResults;

            if(this->alternativesAlgs[ALTERNATIVES_PLATEAU_ALGORITHM]== ptrAlg) {
                testResults.push_back(Routing::Tests::LoadTestResult(std::string(QUERY_SET_RESULTS_DIR) + std::string("/00_") + to_string(nodeIdFrom) + std::string("_")
                                                                     + to_string(nodeIdTo) + std::string("_Alternatives.csv")));
                testResults.push_back(Routing::Tests::LoadTestResult(std::string(QUERY_SET_RESULTS_DIR) + std::string("/01_") + to_string(nodeIdFrom) + std::string("_")
                                                                     + to_string(nodeIdTo) + std::string("_Alternatives.csv")));

            } else {
                std::cout<< "No implementation of this Alg" << endl;
                exit(-1);
            }

            return testResults;
        }

        void CheckResult(const std::vector<std::vector<int>>& testResult,
                         std::unique_ptr<std::vector<Routing::Algorithms::Result>> result) {

            if(result == nullptr) {
                ADD_FAILURE() << "Result is empty.";
                return;
            }

            unsigned int i = 0;
            for(auto &res : *result) {
                CheckResult(testResult.at(i), res);
                //break; //comment for control first alternatives path
                i++;
                if(i>1){
                    break;
                }
            }
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

    TEST_F(RoutingTestAlternatives, TestAlternatives){
        /*//Routing::Data::Probability::ProfileStorageHDF5 storage(HDF_PROFILE_PATH, true);
        auto map = Routing::Tests::LoadGraph("/home/faltos/Plocha/IT4I/testy_routing/segments_v3_2803.hdf5");
        Routing::Data::Probability::ProfileStorageHDF5 storage("/home/faltos/Plocha/IT4I/testy_routing/prof_viden.h5", false);
        //Routing::Data::Probability::ProfileStorageHDF5 storage("/home/faltos/Plocha/IT4I/testy_routing/results_prague.h5", false);

        std::clock_t start;
        double duration;
        start = std::clock();

        //83931322 --> 83665073
        Routing::Algorithms::TD::TDDijkstra dijkstraTD(map, storage);

        google::dense_hash_set<int> betweennessEdges;
        betweennessEdges.set_empty_key(std::numeric_limits<int>::min());

        std::vector<std::vector<int>> alternatives;

        auto btw = Routing::Algorithms::TD::GetNumbersFromFile("/home/faltos/Plocha/IT4I/testy_routing/TestTD/btwResViden.txt");
        auto alternatives01 = Routing::Algorithms::TD::GetNumbersFromFile("/home/faltos/Plocha/IT4I/testy_routing/TestTD/alternatives01.txt");
        auto alternatives02 = Routing::Algorithms::TD::GetNumbersFromFile("/home/faltos/Plocha/IT4I/testy_routing/TestTD/alternatives02.txt");
        alternatives.push_back(alternatives01);
        alternatives.push_back(alternatives02);

        for (auto item : btw) {
            betweennessEdges.insert(item);
        }

        Routing::Algorithms::TD::TDDAlternativesUtility tddAlternativesUtility(map,storage);
        Routing::Algorithms::AlternativesPlateauAlgorithm alternativesPlateauAlgorithm(map);
        auto alternativesResult = alternativesPlateauAlgorithm.GetResults(87828320, 88944153, 8, Routing::Algorithms::GraphFilterGeometry::Ellipse, false, true, true);

        for(unsigned int i = 0; i<8; i++){
            cout << "Result[" << i << "]" << endl;
            std::vector<int> altNodes;
            for(unsigned int j = 0; j < alternativesResult->at(i).GetResult().size();j++){
                //cout << alternativesResult->at(i).GetResult().at(j).nodeId1 << ",";
                cout << alternativesResult->at(i).GetResult().at(j).edgeId << ",";
                altNodes.push_back(alternativesResult->at(i).GetResult().at(j).nodeId1);
            }
            alternatives.push_back(altNodes);
            cout << endl;
            cout << endl;
        }

        duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
        std::cout<< std::endl << "Time of init: " << duration << endl;

        start = std::clock();


        auto results = tddAlternativesUtility.GetRoute(betweennessEdges,alternatives, 88222647, 87817376, 25200);

        duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
        std::cout<< std::endl << "Time of algorithm: " << duration << endl;

        for(unsigned int i = 0; i<results.size(); i++){
            cout << "Result[" << i << "]" << endl;
            for(unsigned int j = 0; j < results.at(i).size();j++){
                cout << results.at(i).at(j).edgeId << ", ";
            }
            cout << endl;
            cout << endl;
        }*/

    }

    TEST_F(RoutingTestAlternatives, AlternativesPlateauAlgorithm) {

        //Testing first and second result of alg
        RunAlternativeAlg(this->alternativesAlgs[ALTERNATIVES_PLATEAU_ALGORITHM]);

    }

    TEST_F(RoutingTestAlternatives, AlternativesSimilarityParameter) {
        std::tuple<int, int> query = this->idQuerySet[0];

        Routing::Node n1 = this->routingGraph->GetNodeById(get<0>(query));
        Routing::Node n2 = this->routingGraph->GetNodeById(get<1>(query));

        AlternativesPlateauAlgorithm alg = Routing::Algorithms::AlternativesPlateauAlgorithm(this->routingGraph);

        auto settings = alg.GetAlgorithmSettings();
        settings.SetSimilarity(60);
        settings.SetUseSimilarityParameter(true);
        alg.SetAlgorithmSettings(settings);

        alg.UpdateAlternativesSettings();

        auto result = alg.GetResults(n1.id, n2.id, 4, true);

        if(result->size() != 3){
            ADD_FAILURE() << "Result size is incorrect.";
        }
    }
}

