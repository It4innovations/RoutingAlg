#include "DifferentialEvolution.h"

void DifferentialEvolution::Run(){
    std::cout << "Running...\n" << std::endl;

    LoadRoutingIndex();

    std::vector<DiffEvoIndividual> population;

    population = GenerateFirstPopulation();

    CalculatePopulationCost(population);

    for(int i = 0; i < this->countGeneration;i++){
        auto newGeneration = GenerateNewPopulation(population);
        CalculatePopulationCost(newGeneration);
        BetterIndividualOfGeneration(population, newGeneration);

        if(printInfo) {
            PrintBestIndividual(population, i);
        }

    }


    int index = 0;
    for(auto individual : population){
        std::cout << "Index: " << index++ << std::endl;
        individual.PrintParameters();
    }

}

void DifferentialEvolution::CalculatePopulationCost(std::vector<DiffEvoIndividual> &population){
    #pragma omp parallel for
    for(unsigned int i = 0; i < this->countIndividuals; i++){
        auto alt = Routing::Algorithms::AlternativesPlateauAlgorithm(routingIndex);

        Routing::Algorithms::AlgorithmSettings settings;
        settings.filterSettings.longDistanceValue = population.at(i).fromXKmLongPath;
        settings.filterSettings.firstHierarchyJumpDistance = population.at(i).firstDistanceKmShortPath;
        settings.filterSettings.firstHierarchyJumpLongDistance = population.at(i).firstDistanceKmLongPath;
        settings.filterSettings.secondHierarchyJumpDistance = population.at(i).secondDistanceKmShortPath;
        settings.filterSettings.secondHierarchyJumpLongDistance = population.at(i).secondDistanceKmLongPath;

        alt.SetAlgorithmSettings(settings);

        auto t_start = std::chrono::high_resolution_clock::now();

        //auto res = alt.GetResults(202264520, 201783803, 3, false, 0);
        //res = alt.GetResults(202264520, 201395386, 3, false, 0);

        int routesCount = 0;
        for(auto ids : idQuerySet) {
            auto res = alt.GetResults(get<0>(ids), get<1>(ids), countAlternatives, false, 0);
            if (res != nullptr) {
                int count = 0;
                for (auto &route : *res) {
                    if (route.GetResult().empty()) {
                        break;
                    }
                    count++;
                }
                routesCount += count;
            } else {
                population.at(i).someRouteNotFound = true;
                //std::cout << "someRouteNotFound" << std::endl;
            }
        }

        auto t_end = std::chrono::high_resolution_clock::now();
        double time = std::chrono::duration<double, std::milli>(t_end-t_start).count();
        //std::cout << "Time: " << time << std::endl;

        population.at(i).SetCost(time, routesCount);
    }
}

std::vector<DiffEvoIndividual> DifferentialEvolution::GenerateFirstPopulation(){
    std::vector<DiffEvoIndividual> population;

    for(unsigned int i = 0; i < this->countIndividuals; i++){
        DiffEvoIndividual newIndividual = DiffEvoIndividual();
        int firstDistanceKmLongPath = (rand() % 5) + 1;
        int secondDistanceKmLongPath = (rand() % 20) + 1;
        int firstDistanceKmShortPath = (rand() % 10) + 1;
        int secondDistanceKmShortPath = (rand() % 30) + 1;
        int fromXKmLongPath = (rand() % 100) + 15;
        int similarityPercent = 50;
        newIndividual.SetParameter(firstDistanceKmLongPath, secondDistanceKmLongPath, firstDistanceKmShortPath,
                                   secondDistanceKmShortPath, fromXKmLongPath, similarityPercent);
        population.push_back(newIndividual);
    }

    return population;
}

std::vector<DiffEvoIndividual> DifferentialEvolution::GenerateNewPopulation(std::vector<DiffEvoIndividual> &oldPopulation){
    std::vector<DiffEvoIndividual> newPopulation;

    for(unsigned int i = 0; i < this->countIndividuals; i++){
        auto randomIndex = (unsigned int) (rand() % countIndividuals);
        while (randomIndex == i){
            randomIndex = (unsigned int) (rand() % countIndividuals);
        }
        DiffEvoIndividual newIndividual = DiffEvoIndividual();

        //Mutation -
        auto firstDistanceKmLongPath = int(abs(oldPopulation.at(i).firstDistanceKmLongPath - oldPopulation.at(randomIndex).firstDistanceKmLongPath));
        auto secondDistanceKmLongPath = int(abs(oldPopulation.at(i).secondDistanceKmLongPath - oldPopulation.at(randomIndex).secondDistanceKmLongPath));
        auto firstDistanceKmShortPath = int(abs(oldPopulation.at(i).firstDistanceKmShortPath - oldPopulation.at(randomIndex).firstDistanceKmShortPath));
        auto secondDistanceKmShortPath = int(abs(oldPopulation.at(i).secondDistanceKmShortPath - oldPopulation.at(randomIndex).secondDistanceKmShortPath));
        auto fromXKmLongPath = int(abs(oldPopulation.at(i).fromXKmLongPath - oldPopulation.at(randomIndex).fromXKmLongPath));
        int similarityPercent = 50;

        //Mutation *F
        /*firstDistanceKmLongPath = int(firstDistanceKmLongPath * this->mutation);
        secondDistanceKmLongPath = int(secondDistanceKmLongPath * this->mutation);
        firstDistanceKmShortPath = int(firstDistanceKmShortPath * this->mutation);
        secondDistanceKmShortPath = int(secondDistanceKmShortPath * this->mutation);
        fromXKmLongPath = int(fromXKmLongPath * this->mutation);*/

        //Mutation +
        auto randomIndexSecond = (unsigned int) (rand() % countIndividuals);
        while (randomIndexSecond == i || randomIndexSecond == randomIndex){
            randomIndexSecond = (unsigned int) (rand() % countIndividuals);
        }

        firstDistanceKmLongPath = int (firstDistanceKmLongPath + oldPopulation.at(randomIndexSecond).firstDistanceKmLongPath);
        secondDistanceKmLongPath = int (secondDistanceKmLongPath + oldPopulation.at(randomIndexSecond).secondDistanceKmLongPath);
        firstDistanceKmShortPath = int (firstDistanceKmShortPath + oldPopulation.at(randomIndexSecond).firstDistanceKmShortPath);
        secondDistanceKmShortPath = int (secondDistanceKmShortPath + oldPopulation.at(randomIndexSecond).secondDistanceKmShortPath);
        fromXKmLongPath = int (fromXKmLongPath + oldPopulation.at(randomIndexSecond).fromXKmLongPath);

        //Mutation *F
        firstDistanceKmLongPath = int(firstDistanceKmLongPath * this->mutation);
        secondDistanceKmLongPath = int(secondDistanceKmLongPath * this->mutation);
        firstDistanceKmShortPath = int(firstDistanceKmShortPath * this->mutation);
        secondDistanceKmShortPath = int(secondDistanceKmShortPath * this->mutation);
        fromXKmLongPath = int(fromXKmLongPath * this->mutation);

        //Crossover
        int randomNum = rand() % 100;
        if (randomNum > this->crossover){
            firstDistanceKmLongPath = (int)oldPopulation.at(i).firstDistanceKmLongPath;
        }

        randomNum = rand() % 100;
        if (randomNum > this->crossover){
            secondDistanceKmLongPath = (int)oldPopulation.at(i).secondDistanceKmLongPath;
        }

        randomNum = rand() % 100;
        if (randomNum > this->crossover){
            firstDistanceKmShortPath = (int)oldPopulation.at(i).firstDistanceKmShortPath;
        }

        randomNum = rand() % 100;
        if (randomNum > this->crossover){
            secondDistanceKmShortPath = (int)oldPopulation.at(i).secondDistanceKmShortPath;
        }

        randomNum = rand() % 100;
        if (randomNum > this->crossover){
            fromXKmLongPath = (int)oldPopulation.at(i).fromXKmLongPath;
        }


        //Correction
        if(firstDistanceKmLongPath > secondDistanceKmLongPath){
            secondDistanceKmLongPath = firstDistanceKmLongPath + 1;
        }

        if(firstDistanceKmShortPath > secondDistanceKmShortPath){
            secondDistanceKmShortPath = firstDistanceKmShortPath + 1;
        }


        newIndividual.SetParameter(firstDistanceKmLongPath, secondDistanceKmLongPath, firstDistanceKmShortPath,
                                   secondDistanceKmShortPath, fromXKmLongPath, similarityPercent);
        newPopulation.push_back(newIndividual);
    }

    return newPopulation;
}

void DifferentialEvolution::BetterIndividualOfGeneration(std::vector<DiffEvoIndividual> &originGen,
                                                         std::vector<DiffEvoIndividual> &newGen){
    for(unsigned int i = 0; i < this->countIndividuals; i++){
        if(originGen.at(i).GetCost() > newGen.at(i).GetCost()){
            if(!newGen.at(i).someRouteNotFound) {
                originGen.at(i) = newGen.at(i);
            }
        } else {
            if(originGen.at(i).someRouteNotFound){
                originGen.at(i) = newGen.at(i);
            }
        }
    }
}

void DifferentialEvolution::PrintBestIndividual(std::vector<DiffEvoIndividual> &population, int numberGeneration){
    unsigned int indexBest = 0;
    for(unsigned int i = 0; i < this->countIndividuals; i++){
        if(population.at(i).timeCost < population.at(indexBest).timeCost){
            indexBest = i;
        }
    }
    std::cout << "GENERATION: " << numberGeneration << std::endl;
    std::cout << "Best individual index: " << indexBest << std::endl;
    population.at(indexBest).PrintParameters();
    std::cout << "----------------------------------------" << std::endl << std::endl << std::endl;
}

void DifferentialEvolution::LoadRoutingIndex() {
    //H5::H5File h5file(HDF_INDEX_PATH, H5F_ACC_RDONLY);
    H5::H5File h5file(pathHDF5, H5F_ACC_RDONLY);
    unique_ptr<DataIndex> dif(new Routing::Hdf::DataIndex(h5file));
    h5file.close();
    routingIndex.reset(new Routing::Data::GraphMemory(*dif));
}