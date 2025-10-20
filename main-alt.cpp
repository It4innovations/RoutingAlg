#include "Routing/Algorithms/Alternatives/AlternativesPlateauAlgorithm.h"
#include "Routing/Data/Probability/ProfileStorageHDF5.h"
#include "Routing/Tests/IOUtils.h"
#include <iostream>


int main(int argc, char **argv) {

  const shared_ptr<Routing::Data::GraphMemory> routingGraph =
      Routing::Tests::LoadGraph(
          "/home/pav/Repos/mpi/project/data2/map_1748244949128_135415.hdf5");

  auto settings = Routing::Algorithms::AlgorithmSettings();
  settings.SetSimilarity(55);
  settings.filterSettings.allFilterOff = true;
  auto algTmp = new Routing::Algorithms::AlternativesPlateauAlgorithm(routingGraph);
  const Routing::Algorithms::AlternativesPlateauAlgorithm* alg = algTmp;

  int id_origin = 74152;
  int id_destination = 46097;
  int max_routes = 10;
  std::unique_ptr<std::vector<Result>> results = alg->GetResults(id_origin, id_destination, max_routes, false);

  if (results == nullptr) {
    std::cout << "No solutions." << std::endl;
  } else {
    std::ofstream output("../data/routes.csv");

    output << "Routes as sequences of osmn ids\n";

    for (auto &res : *results) {
      std::cout << res << std::endl;

      res.CSVRoutes(output);
    }
  }
  return EXIT_SUCCESS;
}
