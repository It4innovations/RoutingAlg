#include "Routing/Algorithms/Alternatives/AlternativesPlateauAlgorithm.h"
#include "Routing/Data/Probability/ProfileStorageHDF5.h"
#include "Routing/Tests/IOUtils.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>


int main(int argc, char **argv) {

  if (argc < 5 || argc > 6) {
    std::cerr << "Usage: " << argv[0]
              << " <map.hdf5> <origin-node-id> <destination-node-id>"
              << " <none|ellipse|hierarchy|hierarchy-f6-f5|hierarchy-f7-f6|"
                 "hierarchy-f6-f6|hierarchy-f6-f5-late|hierarchy-f7-f6-late|"
                 "hierarchy-f6-f5-medium|hierarchy-open> [max-routes]"
              << std::endl;
    return EXIT_FAILURE;
  }

  const shared_ptr<Routing::Data::GraphMemory> routingGraph =
      Routing::Tests::LoadGraph(argv[1]);

  const std::string filterMode = argv[4];
  auto settings = Routing::Algorithms::AlgorithmSettings();
  settings.SetSimilarity(55);
  if (filterMode == "none") {
    settings.filterSettings.allFilterOff = true;
  } else if (filterMode == "ellipse" || filterMode == "hierarchy" ||
             filterMode == "hierarchy-f6-f5" || filterMode == "hierarchy-f7-f6" ||
             filterMode == "hierarchy-f6-f6" || filterMode == "hierarchy-f6-f5-late" ||
             filterMode == "hierarchy-f6-f5-medium" ||
             filterMode == "hierarchy-f7-f6-late" || filterMode == "hierarchy-open") {
    settings.filterSettings.allFilterOff = false;
    settings.filterSettings.typeOfFilter =
        Routing::Algorithms::GraphFilterGeometry::Ellipse;
    settings.filterSettings.hierarchyStartPositionFilterOn = filterMode != "ellipse";

    if (filterMode == "hierarchy-f6-f5") {
      settings.filterSettings.firstHierarchyJumpFRC = 6;
      settings.filterSettings.secondHierarchyJumpFRC = 5;
    } else if (filterMode == "hierarchy-f6-f6") {
      settings.filterSettings.firstHierarchyJumpFRC = 6;
      settings.filterSettings.secondHierarchyJumpFRC = 6;
    } else if (filterMode == "hierarchy-f6-f5-late") {
      settings.filterSettings.firstHierarchyJumpFRC = 6;
      settings.filterSettings.secondHierarchyJumpFRC = 5;
      settings.filterSettings.firstHierarchyJumpDistance = 30;
      settings.filterSettings.secondHierarchyJumpDistance = 120;
      settings.filterSettings.firstHierarchyJumpLongDistance = 30;
      settings.filterSettings.secondHierarchyJumpLongDistance = 100;
    } else if (filterMode == "hierarchy-f6-f5-medium") {
      settings.filterSettings.firstHierarchyJumpFRC = 6;
      settings.filterSettings.secondHierarchyJumpFRC = 5;
      settings.filterSettings.firstHierarchyJumpDistance = 20;
      settings.filterSettings.secondHierarchyJumpDistance = 100;
      settings.filterSettings.firstHierarchyJumpLongDistance = 15;
      settings.filterSettings.secondHierarchyJumpLongDistance = 60;
    } else if (filterMode == "hierarchy-f7-f6" || filterMode == "hierarchy-f7-f6-late") {
      settings.filterSettings.firstHierarchyJumpFRC = 7;
      settings.filterSettings.secondHierarchyJumpFRC = 6;

      if (filterMode == "hierarchy-f7-f6-late") {
        settings.filterSettings.firstHierarchyJumpDistance = 30;
        settings.filterSettings.secondHierarchyJumpDistance = 120;
        settings.filterSettings.firstHierarchyJumpLongDistance = 30;
        settings.filterSettings.secondHierarchyJumpLongDistance = 100;
      }
    } else if (filterMode == "hierarchy-open") {
      settings.filterSettings.firstHierarchyJumpFRC = 7;
      settings.filterSettings.secondHierarchyJumpFRC = 7;
    } else if (filterMode != "hierarchy" && filterMode != "ellipse") {
      std::cerr << "Unknown hierarchy profile: " << filterMode << std::endl;
      return EXIT_FAILURE;
    }
  } else {
    std::cerr << "Unknown filter mode: " << filterMode << std::endl;
    return EXIT_FAILURE;
  }
  auto algTmp = new Routing::Algorithms::AlternativesPlateauAlgorithm(routingGraph, settings);
  const Routing::Algorithms::AlternativesPlateauAlgorithm* alg = algTmp;

  int id_origin = std::stoi(argv[2]);
  int id_destination = std::stoi(argv[3]);
  int max_routes = argc == 6 ? std::stoi(argv[5]) : 10;
  const auto queryStart = std::chrono::steady_clock::now();
  std::unique_ptr<std::vector<Result>> results = alg->GetResults(id_origin, id_destination, max_routes, false);
  const auto queryDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now() - queryStart);
  std::cout << "Filter query time: " << queryDuration.count() << " ms" << std::endl;

  if (results == nullptr) {
    std::cout << "No solutions." << std::endl;
  } else {
    std::ofstream output("routes-" + filterMode + ".csv");

    output << "Routes as sequences of osmn ids\n";

    for (auto &res : *results) {
      std::cout << res << std::endl;

      res.CSVRoutes(output);
    }
  }
  return EXIT_SUCCESS;
}
