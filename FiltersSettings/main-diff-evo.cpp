#include "DifferentialEvolution.h"
#include <inih/INIReader.h>

int main(int argc, char **argv) {
    string alternativesSettingsPath = "../../../routing/graph-algorithms/FiltersSettings/alternativesSettings.ini";

    if(argc > 2){
        std::cout << "Input failure\n" << "For help run program with parameter -h" << std::endl;

        return 1;
    }

    if(argc == 2){
        if(strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "help") == 0){
            std::cout << "Help for filters-diff-evo:" << std::endl;
            std::cout << "Run program with none parameters for default settings" << std::endl;
            std::cout << "./filters-diff-evo" << std::endl << std::endl;
            std::cout << "Run program with one parameter (path to settings file) for your own setting" << std::endl;
            std::cout << "./filters-diff-evo [pathToFile]" << std::endl << std::endl;
            return 0;
        }

        alternativesSettingsPath = argv[1];
    }

    INIReader alternativesSettingsReader(alternativesSettingsPath);

    if (alternativesSettingsReader.ParseError() < 0) {
        std::cout << "Can't load 'alternativesSettings.ini'\n";
        return 1;
    }

    int countIndividual, countGeneration, countAlternatives, crossover;
    double mutation;
    bool useSimilarityParameter;
    string pathHDF5, pathCities;

    countIndividual = (int) alternativesSettingsReader.GetInteger("settings", "countIndividual", 16);
    countGeneration = (int) alternativesSettingsReader.GetInteger("settings", "countGeneration", 100);
    countAlternatives = (int) alternativesSettingsReader.GetInteger("settings", "countAlternatives", 3);
    mutation = alternativesSettingsReader.GetReal("settings", "mutation", 0.8);
    crossover = (int) alternativesSettingsReader.GetInteger("settings", "crossover", 50);
    useSimilarityParameter = alternativesSettingsReader.GetBoolean("settings", "useSimilarityParameter", false);
    pathHDF5 = alternativesSettingsReader.Get(
            "settings", "pathHDF5", "../../../routing/graph-algorithms/data/routing_indexes/CZE-1528295206-proc-20180724134457.hdf");
    pathCities = alternativesSettingsReader.Get(
            "settings", "pathCities", "../../../routing/graph-algorithms/FiltersSettings/DiffEvoCities.csv");

    std::cout << "Configuration:"
              << "\n  countIndividual: " << countIndividual
              << "\n  countGeneration: " << countGeneration
              << "\n  countAlternatives: " << countAlternatives
              << "\n  mutation: " << mutation
              << "\n  crossover: " << crossover
              << "\n  useSimilarityParameter: " << useSimilarityParameter
              << "\n  pathHDF5: " << pathHDF5
              << "\n  pathCities: " << pathCities << "\n\n";


    DifferentialEvolution difEvo = DifferentialEvolution((unsigned int)countIndividual, countGeneration, countAlternatives,
                                                         mutation, crossover, useSimilarityParameter, pathHDF5, pathCities);

    difEvo.Run();
}
