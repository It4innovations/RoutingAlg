#include "Routing/Algorithms/Alternatives/AlternativesPlateauAlgorithm.h"
#include "Routing/Tests/IOUtils.h"

#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>

int main() {
  // Edit these values, rebuild routing-alt, and run it without arguments.
  const char *map_path = "data/graph.hdf5";
  const int origin = 3916;
  const int destination = 1114;
  const unsigned int max_routes = 10;

  Routing::Algorithms::AlgorithmSettings settings;

  settings.filterSettings.allFilterOff = false;
  settings.filterSettings.typeOfFilter =
      Routing::Algorithms::GraphFilterGeometry::Ellipse;
  settings.filterSettings.hierarchyStartPositionFilterOn = true;

  try {
    const auto graph = Routing::Tests::LoadGraph(map_path);
    const Routing::Algorithms::AlternativesPlateauAlgorithm algorithm(graph, settings);

    const auto query_start = std::chrono::steady_clock::now();
    const auto routes = algorithm.GetResults(origin, destination, max_routes, false);
    const auto query_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - query_start);

    std::cout << "Query time: " << query_time.count() << " ms\n";

    if (routes == nullptr || routes->empty()) {
      std::cout << "No routes found.\n";
      return EXIT_SUCCESS;
    }

    std::cout << "Found " << routes->size() << " route(s).\n";
    for (const auto &route : *routes) std::cout << route << '\n';
  } catch (const std::exception &error) {
    std::cerr << "Error: " << error.what() << '\n';
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
