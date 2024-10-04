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
#include "../Routing/Algorithms/Alternatives/AlternativesPlateauAlgorithm.h"
#include "../Routing/Data/Indexes/GraphMemory.h"
#include "DataPaths.h"
#include "Environment.h"

namespace {

    class RoutingTestTime : public ::testing::Test {

    protected:
        virtual void SetUp() {
            this->routingGraph = GRAPH_ENV->routingGraph;
            auto idQueries = Routing::Tests::LoadIdQuerySet(ID_QUERY_SET_TIME_TEST);

            this->algs.push_back(new Routing::Algorithms::Dijkstra(this->routingGraph));
            this->algs.push_back(new Routing::Algorithms::BidirectionalDijkstra(this->routingGraph));
            this->algs.push_back(new Routing::Algorithms::Astar(this->routingGraph));
            this->algs.push_back(new Routing::Algorithms::BidirectionalAstar(this->routingGraph));

            this->alternativesAlgs.push_back(new Routing::Algorithms::AlternativesPlateauAlgorithm(this->routingGraph));

            for (const auto &query: idQueries) {
                this->idQuerySet.push_back(query);
            }
        }

        shared_ptr <GraphMemory> routingGraph;
        std::vector<std::tuple<int, int>> idQuerySet;
        std::vector<Routing::Algorithms::StaticRoutingAlgorithm *> algs;
        std::vector<Routing::Algorithms::AlternativesAlgorithm *> alternativesAlgs;

        string CitiesFrom[4] = {"Praha", "Ostrava", "Praha", "Vitkov"};
        string CitiesTo[4] = {"Praha", "Ostrava", "Ostrava", "Opava"};


        void RunAlg(Routing::Algorithms::StaticRoutingAlgorithm * ptrAlg){
            int indexCities = 0;
            for (std::tuple<int, int> &query: this->idQuerySet) {

                Routing::Node n1 = this->routingGraph->GetNodeById(get<0>(query));
                Routing::Node n2 = this->routingGraph->GetNodeById(get<1>(query));

                ptrAlg[0].GetResult(n1.id, n2.id, CostCalcType::Time_cost);

                indexCities++;
            }
        }

        void RunAlternativeAlg(Routing::Algorithms::AlternativesAlgorithm * ptrAlg, int typeOfFilter){
            int indexCities = 0;
            for (std::tuple<int, int> &query: this->idQuerySet) {

                Routing::Node n1 = this->routingGraph->GetNodeById(get<0>(query));
                Routing::Node n2 = this->routingGraph->GetNodeById(get<1>(query));

                switch (typeOfFilter){
                    case 0:
                        alternativesAlgs[0]->GetAlgorithmSettings().SetFilterType(Routing::Algorithms::GraphFilterGeometry::NotSet);
                        alternativesAlgs[0]->GetResults(n1.id, n2.id, 3, true);
                        break;
                    case 1:
                        alternativesAlgs[0]->GetAlgorithmSettings().SetFilterType(Routing::Algorithms::GraphFilterGeometry::Circle);
                        alternativesAlgs[0]->GetResults(n1.id, n2.id, 3, true);
                        break;
                    case 2:
                        alternativesAlgs[0]->GetAlgorithmSettings().SetFilterType(Routing::Algorithms::GraphFilterGeometry::Ellipse);
                        alternativesAlgs[0]->GetResults(n1.id, n2.id, 3, true);
                        break;
                }

                indexCities++;
            }
        }
    };

    TEST_F(RoutingTestTime, Dijkstra) {

        RunAlg(algs[DIJKSTRA]);
    }

    TEST_F(RoutingTestTime, BidirectionalDijkstra) {

        RunAlg(algs[BIDIRECTION_DIJKSTRA]);

    }

    TEST_F(RoutingTestTime, Astar) {

        RunAlg(algs[ASTAR]);

    }

    TEST_F(RoutingTestTime, BidirectionalAstar) {

        RunAlg(algs[BIDIRECTION_ASTAR]);

    }

    TEST_F(RoutingTestTime, AlternativesPlateauAlgorithm) {

        RunAlternativeAlg(this->alternativesAlgs[ALTERNATIVES_PLATEAU_ALGORITHM], 0);

    }

    TEST_F(RoutingTestTime, AlternativesPlateauAlgorithmCircle) {

        RunAlternativeAlg(this->alternativesAlgs[ALTERNATIVES_PLATEAU_ALGORITHM], 1);

    }

    TEST_F(RoutingTestTime, AlternativesPlateauAlgorithmElipse) {

        RunAlternativeAlg(this->alternativesAlgs[ALTERNATIVES_PLATEAU_ALGORITHM], 2);

    }

    TEST_F(RoutingTestTime, AlternativesPlateauAlgorithmElipseMoreCities) {

        string CitiesFrom[20] = {"Praha", "Ostrava", "Brno", "Liberec", "Pardubice", "Hradec Kralove", "Ceske Budejovice", "Olomouc", "Prerov", "Opava",
                                "Praha", "Praha", "Praha", "Brno", "Brno", "Pardubice", "Pardubice", "Pardubice", "Olomouc", "Prerov"};
        string CitiesTo[20] = {"Praha", "Ostrava", "Brno", "Liberec", "Pardubice", "Hradec Kralove", "Ceske Budejovice", "Olomouc", "Prerov", "Opava",
                               "Brno", "Ostrava", "Ceske Budejovice", "Olomouc", "Ceske Budejovice", "Hradec Kralove", "Olomouc", "Opava", "Opava", "Liberec"};

        int CitiesFromNodeID[20] = {201556208, 202239996, 201434069, 201701809, 201955383, 201527263, 202423534, 202028664, 201629927, 201962763,
                                    201556208, 201556208, 201556208, 201434069, 201434069, 201955383, 201955383, 201955383, 202028664, 201629927};
        int CitiesToNodeID[20] = {201826796, 202319749, 201872319, 202042984, 202245520, 202185623, 201444369, 201533687, 202035242, 202039919,
                                  201872319, 202319749, 201444369, 201533687, 201444369, 202185623, 201533687, 202039919, 202039919, 202042984};

        for(int i = 0;i < 20; i++){
            alternativesAlgs[0]->GetResults(CitiesFromNodeID[i], CitiesToNodeID[i]);
        }
    }
}
