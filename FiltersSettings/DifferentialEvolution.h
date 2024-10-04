#pragma once

#include <iostream>
#include <vector>
#include <H5Cpp.h>
#include <memory>
#include "DiffEvoIndividual.h"
#include "../Tests/DataPaths.h"
#include "../Routing/Hdf/DataIndex.h"
#include "../Routing/Data/Indexes/GraphMemory.h"
#include "../Routing/Algorithms/Base/StaticRoutingAlgorithm.h"
#include "../Routing/Algorithms/Alternatives/AlternativesPlateauAlgorithm.h"
#include "../Routing/Tests/IOUtils.h"

class DifferentialEvolution {
public:
    DifferentialEvolution(unsigned int countIndividual, int countGeneration, int countAlternatives, double mutation,
                          int crossover, bool useSimilarityParameter, string pathHDF5, string pathCities){
        this->countIndividuals = countIndividual;
        this->countGeneration = countGeneration;
        this->countAlternatives = countAlternatives;
        this->mutation = mutation;
        this->crossover = crossover; //0-100%
        this->useSimilarityParameter = useSimilarityParameter;
        this->pathHDF5 = pathHDF5;

        //auto idQueries = Routing::Tests::LoadIdQuerySet("../../../routing/graph-algorithms/FiltersSettings/DiffEvoCities.csv");
        auto idQueries = Routing::Tests::LoadIdQuerySet(pathCities);

        int count = 0;
        for (const auto &query: idQueries) {
            this->idQuerySet.push_back(query);
            count++;
        }

        if(count == 0){
            std::cout << "No Cities in query" << std::endl;
            exit(1);
        }

        expectedRoutesCount = count * countAlternatives;
        printInfo = true;
    }

    void Run();

private:
    unsigned int countIndividuals;
    int countGeneration;
    int countAlternatives;
    double mutation;
    int crossover;
    bool useSimilarityParameter;
    int expectedRoutesCount;
    bool printInfo;
    string pathHDF5;

    std::vector<std::tuple<int, int>> idQuerySet;

    std::shared_ptr<Routing::Data::GraphMemory> routingIndex;

    void LoadRoutingIndex();

    void CalculatePopulationCost(std::vector<DiffEvoIndividual> &population);

    void BetterIndividualOfGeneration(std::vector<DiffEvoIndividual> &originGen, std::vector<DiffEvoIndividual> &newGen);

    void PrintBestIndividual(std::vector<DiffEvoIndividual> &population, int numberGeneration);

    std::vector<DiffEvoIndividual> GenerateFirstPopulation();

    std::vector<DiffEvoIndividual> GenerateNewPopulation(std::vector<DiffEvoIndividual> &oldPopulation);

};
