#include "Routing/Algorithms/Alternatives/AlternativesPlateauAlgorithm.h"
#include "Routing/Tests/IOUtils.h"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using Algorithm = Routing::Algorithms::AlternativesPlateauAlgorithm;
using Clock = std::chrono::steady_clock;

class NullBuffer final : public std::streambuf {
 protected:
  int overflow(int character) override { return character; }
};

struct Mode {
  std::string name;
  std::unique_ptr<Algorithm> algorithm;
  bool filtered;
};

struct QueryResult {
  double milliseconds = 0.0;
  std::size_t routes = 0;
  std::string error;
};

Routing::Algorithms::AlgorithmSettings SettingsFor(const std::string &mode) {
  Routing::Algorithms::AlgorithmSettings settings;
  settings.SetSimilarity(55);
  if (mode == "none") {
    settings.filterSettings.allFilterOff = true;
    return settings;
  }
  settings.filterSettings.allFilterOff = false;
  settings.filterSettings.typeOfFilter =
      Routing::Algorithms::GraphFilterGeometry::Ellipse;
  settings.filterSettings.hierarchyStartPositionFilterOn = mode != "ellipse";
  if (mode == "hierarchy-tuned") {
    settings.filterSettings.firstHierarchyJumpFRC = 6;
    settings.filterSettings.secondHierarchyJumpFRC = 5;
    settings.filterSettings.firstHierarchyJumpDistance = 20;
    settings.filterSettings.secondHierarchyJumpDistance = 100;
    settings.filterSettings.firstHierarchyJumpLongDistance = 15;
    settings.filterSettings.secondHierarchyJumpLongDistance = 60;
  } else if (mode != "ellipse" && mode != "hierarchy") {
    throw std::invalid_argument("unknown mode: " + mode);
  }
  return settings;
}

QueryResult Run(const Algorithm &algorithm, int origin, int destination,
                unsigned int max_routes) {
  QueryResult query;
  const auto started = Clock::now();
  try {
    const auto results = algorithm.GetResults(origin, destination, max_routes, false);
    query.routes = results == nullptr ? 0 : results->size();
  } catch (const std::exception &error) {
    query.error = error.what();
  } catch (...) {
    query.error = "unknown exception";
  }
  query.milliseconds =
      std::chrono::duration<double, std::milli>(Clock::now() - started).count();
  return query;
}

std::string CsvEscape(const std::string &value) {
  if (value.find_first_of(",\"\n\r") == std::string::npos) return value;
  std::string escaped = "\"";
  for (const char character : value) {
    if (character == '\"') escaped += '\"';
    escaped += character;
  }
  escaped += "\"";
  return escaped;
}
}  // namespace

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0]
              << " MAP.hdf5 PAIRS.tsv OUTPUT.csv MAX_ROUTES\n";
    return EXIT_FAILURE;
  }
  const unsigned int max_routes = std::stoul(argv[4]);
  if (max_routes == 0) {
    std::cerr << "MAX_ROUTES must be positive\n";
    return EXIT_FAILURE;
  }

  const auto graph = Routing::Tests::LoadGraph(argv[1]);
  const std::vector<std::string> mode_names = {
      "none", "ellipse", "hierarchy"};
  std::vector<Mode> modes;
  for (const auto &name : mode_names) {
    modes.push_back({name, std::make_unique<Algorithm>(graph, SettingsFor(name)),
                     name != "none"});
  }

  std::ifstream pairs(argv[2]);
  std::ofstream output(argv[3]);
  if (!pairs) throw std::runtime_error("cannot open pair input");
  if (!output) throw std::runtime_error("cannot open result output");
  output << "pair_index,origin,destination,mode,primary_ms,primary_routes,"
            "fallback_run,fallback_ms,fallback_routes,error\n";

  NullBuffer null_buffer;
  std::streambuf *original_cout = std::cout.rdbuf(&null_buffer);
  std::string line;
  std::size_t processed = 0;
  while (std::getline(pairs, line)) {
    if (line.empty()) continue;
    std::istringstream fields(line);
    long long pair_index;
    int origin;
    int destination;
    if (!(fields >> pair_index >> origin >> destination)) {
      std::cerr << "Invalid pair line: " << line << '\n';
      std::cout.rdbuf(original_cout);
      return EXIT_FAILURE;
    }
    for (const auto &mode : modes) {
      const QueryResult primary =
          Run(*mode.algorithm, origin, destination, max_routes);
      QueryResult fallback;
      const bool fallback_run =
          mode.filtered && primary.error.empty() && primary.routes == 0;
      if (fallback_run) {
        fallback = Run(*modes.front().algorithm, origin, destination, max_routes);
      }
      std::string error = primary.error;
      if (!fallback.error.empty()) {
        if (!error.empty()) error += "; ";
        error += "fallback: " + fallback.error;
      }
      output << pair_index << ',' << origin << ',' << destination << ','
             << mode.name << ',' << primary.milliseconds << ','
             << primary.routes << ',' << (fallback_run ? 1 : 0) << ','
             << fallback.milliseconds << ',' << fallback.routes << ','
             << CsvEscape(error) << '\n';
    }
    ++processed;
    if (processed % 25 == 0) std::cerr << "processed " << processed << " pairs\n";
  }
  std::cout.rdbuf(original_cout);
  std::cerr << "completed " << processed << " pairs\n";
  return EXIT_SUCCESS;
}
