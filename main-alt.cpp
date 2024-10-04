#include <iostream>
#include "Routing/Tests/IOUtils.h"
#include "Routing/Data/Probability/ProfileStorageHDF5.h"
#include "Tests/DataPaths.h"
#include "Routing/Algorithms/Alternatives/AlternativesPlateauAlgorithm.h"


int main(int argc, char **argv) {

    shared_ptr<Routing::Data::GraphMemory> routingGraph = Routing::Tests::LoadGraph("../data/graph.hdf5");

    auto alg = new Routing::Algorithms::AlternativesPlateauAlgorithm(routingGraph);

    auto settings = alg->GetAlgorithmSettings();

    // Similarity - how much similiar the roads should be
    settings.SetSimilarity(55);
    settings.SetUseSimilarityParameter(true);

    // Filtering elipse
    settings.filterSettings.allFilterOff = true;

    // Set settings
    alg->SetAlgorithmSettings(settings);
    alg->UpdateAlternativesSettings();

    int id_origin = 14755;
    int id_destination = 3742;
    int max_routes = 10;
    std::unique_ptr<std::vector<Result>> results = alg->GetResults(id_origin, id_destination, max_routes, false);

    if(results == nullptr){
        std::cout << "No solutions." << std::endl;
    }
    else{
        std::ofstream output("../data/routes.csv");

        output << "Routes as sequences of osmn ids\n";

        for(auto &res : *results) {
            std::cout << res << std::endl;

            res.CSVRoutes(output);
        }
    }
    return EXIT_SUCCESS;
}
