#include "Routing/Algorithms/Alternatives/AlternativesPlateauAlgorithm.h"
#include "Routing/Tests/IOUtils.h"

#include <H5Cpp.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {
using Algorithm = Routing::Algorithms::AlternativesPlateauAlgorithm;
using Clock = std::chrono::steady_clock;
using Result = Routing::Algorithms::Result;
namespace fs = std::filesystem;

struct FilterProfile {
  std::string name;
  Routing::Algorithms::AlgorithmSettings settings;
  unsigned int max_routes = 10;
};

const unsigned int kWarmups = 1;
const unsigned int kRepetitions = 3;

Routing::Algorithms::AlgorithmSettings BaseSettings() {
  Routing::Algorithms::AlgorithmSettings settings;
  return settings;
}

FilterProfile NoFilters(std::string name = "none", unsigned int max_routes = 10) {
  auto settings = BaseSettings();
  settings.filterSettings.allFilterOff = true;
  return {std::move(name), settings, max_routes};
}

FilterProfile EllipseOnly(std::string name = "ellipse", unsigned int max_routes = 10) {
  auto settings = BaseSettings();
  settings.filterSettings.allFilterOff = false;
  settings.filterSettings.typeOfFilter =
      Routing::Algorithms::GraphFilterGeometry::Ellipse;
  settings.filterSettings.hierarchyStartPositionFilterOn = false;
  return {std::move(name), settings, max_routes};
}

FilterProfile HierarchyOnly(std::string name = "hierarchy", unsigned int max_routes = 10) {
  auto settings = BaseSettings();
  settings.filterSettings.allFilterOff = false;
  settings.filterSettings.typeOfFilter =
      Routing::Algorithms::GraphFilterGeometry::NotSet;
  settings.filterSettings.hierarchyStartPositionFilterOn = true;
  return {std::move(name), settings, max_routes};
}

FilterProfile EllipseHierarchy(std::string name = "ellipse-hierarchy",
                               unsigned int max_routes = 10) {
  auto settings = BaseSettings();
  settings.filterSettings.allFilterOff = false;
  settings.filterSettings.typeOfFilter =
      Routing::Algorithms::GraphFilterGeometry::Ellipse;
  settings.filterSettings.hierarchyStartPositionFilterOn = true;
  return {std::move(name), settings, max_routes};
}

std::vector<FilterProfile> BuiltInProfiles() {
  std::vector<FilterProfile> profiles;
  profiles.push_back(NoFilters());
  profiles.push_back(EllipseOnly());
  profiles.push_back(HierarchyOnly());
  profiles.push_back(EllipseHierarchy());

  // Fully custom example. Uncomment to benchmark it.
  // auto custom = EllipseHierarchy("medium-7-6-5");
  // custom.settings.filterSettings.minFRC = 7;
  // custom.settings.filterSettings.firstHierarchyJumpFRC = 6;
  // custom.settings.filterSettings.secondHierarchyJumpFRC = 5;
  // custom.settings.filterSettings.firstHierarchyJumpDistance = 20;
  // custom.settings.filterSettings.secondHierarchyJumpDistance = 100;
  // custom.settings.filterSettings.firstHierarchyJumpLongDistance = 15;
  // custom.settings.filterSettings.secondHierarchyJumpLongDistance = 60;
  // custom.settings.filterSettings.longDistanceValue = 120;
  // profiles.push_back(custom);
  return profiles;
}
struct Options {
  fs::path map_path;
  fs::path od_path;
  fs::path output_directory;
  unsigned int warmups = kWarmups;
  unsigned int repetitions = kRepetitions;
  std::optional<unsigned int> max_routes;
  bool export_paths = false;
  std::vector<std::string> profile_names;
};

struct OdPair {
  std::string pair_id;
  int origin = 0;
  int destination = 0;
  std::string origin_osm_id;
  std::string destination_osm_id;
  std::string flight_distance_km;
  std::string distance_band;
};

struct QueryOutcome {
  double milliseconds = 0.0;
  std::unique_ptr<std::vector<Result>> results;
  std::string error;
};

struct ProfileRuntime {
  FilterProfile profile;
  std::unique_ptr<Algorithm> algorithm;
  double setup_ms = 0.0;
  std::vector<double> measured_times;
  std::size_t ok = 0;
  std::size_t no_route = 0;
  std::size_t errors = 0;
};

struct NullBuffer final : public std::streambuf {
  int overflow(int character) override { return character; }
};

class ScopedCoutSilencer {
 public:
  ScopedCoutSilencer() : original_(std::cout.rdbuf(&buffer_)) {}
  ~ScopedCoutSilencer() { std::cout.rdbuf(original_); }

 private:
  NullBuffer buffer_;
  std::streambuf *original_;
};

double Milliseconds(Clock::duration duration) {
  return std::chrono::duration<double, std::milli>(duration).count();
}

std::string CsvEscape(const std::string &value) {
  if (value.find_first_of(",\"\n\r") == std::string::npos) return value;
  std::string escaped = "\"";
  for (char character : value) {
    if (character == '\"') escaped += '\"';
    escaped += character;
  }
  return escaped + "\"";
}

std::vector<std::string> Split(const std::string &value, char delimiter) {
  std::vector<std::string> parts;
  std::istringstream input(value);
  std::string part;
  while (std::getline(input, part, delimiter)) {
    if (!part.empty()) parts.push_back(part);
  }
  return parts;
}

std::vector<std::string> ParseCsvLine(const std::string &line) {
  std::vector<std::string> fields;
  std::string field;
  bool quoted = false;
  for (std::size_t index = 0; index < line.size(); ++index) {
    const char character = line[index];
    if (character == '\"') {
      if (quoted && index + 1 < line.size() && line[index + 1] == '\"') {
        field += '\"';
        ++index;
      } else {
        quoted = !quoted;
      }
    } else if (character == ',' && !quoted) {
      fields.push_back(field);
      field.clear();
    } else {
      field += character;
    }
  }
  if (quoted) throw std::runtime_error("unterminated quoted CSV field");
  fields.push_back(field);
  return fields;
}

int ParseInt(const std::string &value, const std::string &label) {
  std::size_t parsed = 0;
  const long result = std::stol(value, &parsed);
  if (parsed != value.size() || result < std::numeric_limits<int>::min() ||
      result > std::numeric_limits<int>::max()) {
    throw std::invalid_argument(label + " must be an int32 value: " + value);
  }
  return static_cast<int>(result);
}

unsigned int ParseUnsigned(const std::string &value, const std::string &label,
                           bool allow_zero = false) {
  std::size_t parsed = 0;
  const unsigned long result = std::stoul(value, &parsed);
  if (parsed != value.size() || result > std::numeric_limits<unsigned int>::max() ||
      (!allow_zero && result == 0)) {
    throw std::invalid_argument(label + (allow_zero ? " must be non-negative: "
                                                        : " must be positive: ") +
                                value);
  }
  return static_cast<unsigned int>(result);
}

void PrintUsage(const char *program) {
  std::cout
      << "Usage:\n"
      << "  " << program
      << " --map MAP --od PAIRS.csv --output-dir DIR [options]\n\n"
      << "Options:\n"
      << "  --profiles NAME,NAME   select built-in filter profiles\n"
      << "  --max-routes N         override each profile's route count\n"
      << "  --warmups N            complete warmup passes\n"
      << "  --repetitions N         measured passes\n"
      << "  --export-paths          write bulk route-node rows after timing\n"
      << "  --list-profiles         print configured profiles\n"
      << "  --help                  show this help\n";
}

Options ParseOptions(int argc, char **argv) {
  if (argc == 1) {
    PrintUsage(argv[0]);
    std::exit(EXIT_SUCCESS);
  }
  const std::string first_argument = argv[1];
  if (first_argument == "--help" || first_argument == "-h") {
    PrintUsage(argv[0]);
    std::exit(EXIT_SUCCESS);
  }
  if (first_argument == "--list-profiles") {
    for (const auto &profile : BuiltInProfiles()) std::cout << profile.name << '\n';
    std::exit(EXIT_SUCCESS);
  }

  Options options;
  for (int index = 1; index < argc; ++index) {
    const std::string argument = argv[index];
    auto value = [&]() -> std::string {
      if (++index >= argc) throw std::invalid_argument("missing value after " + argument);
      return argv[index];
    };
    if (argument == "--map") {
      options.map_path = value();
    } else if (argument == "--od") {
      options.od_path = value();
    } else if (argument == "--output-dir") {
      options.output_directory = value();
    } else if (argument == "--warmups") {
      options.warmups = ParseUnsigned(value(), "warmups", true);
    } else if (argument == "--repetitions") {
      options.repetitions = ParseUnsigned(value(), "repetitions");
    } else if (argument == "--max-routes") {
      options.max_routes = ParseUnsigned(value(), "max-routes");
    } else if (argument == "--profiles") {
      options.profile_names = Split(value(), ',');
    } else if (argument == "--export-paths") {
      options.export_paths = true;
    } else if (argument == "--help" || argument == "-h") {
      PrintUsage(argv[0]);
      std::exit(EXIT_SUCCESS);
    } else {
      throw std::invalid_argument("unknown argument: " + argument);
    }
  }
  if (options.map_path.empty()) throw std::invalid_argument("--map is required");
  if (options.od_path.empty()) throw std::invalid_argument("--od is required");
  if (options.output_directory.empty()) throw std::invalid_argument("--output-dir is required");
  return options;
}

std::vector<FilterProfile> SelectProfiles(const Options &options) {
  auto profiles = BuiltInProfiles();
  std::map<std::string, FilterProfile> available;
  for (const auto &profile : profiles) {
    if (profile.name.empty() || profile.name.find(',') != std::string::npos) {
      throw std::invalid_argument("profile names must be non-empty and contain no comma");
    }
    if (!available.emplace(profile.name, profile).second) {
      throw std::invalid_argument("duplicate profile name: " + profile.name);
    }
  }
  if (options.profile_names.empty()) return profiles;
  std::vector<FilterProfile> selected;
  for (const auto &name : options.profile_names) {
    const auto found = available.find(name);
    if (found == available.end()) throw std::invalid_argument("unknown profile: " + name);
    selected.push_back(found->second);
  }
  return selected;
}

std::vector<OdPair> LoadOdPairs(const fs::path &path) {
  std::ifstream input(path);
  if (!input) throw std::runtime_error("cannot open OD CSV: " + path.string());
  std::string line;
  if (!std::getline(input, line)) throw std::runtime_error("OD CSV is empty");
  const auto header = ParseCsvLine(line);
  std::map<std::string, std::size_t> columns;
  for (std::size_t index = 0; index < header.size(); ++index) columns[header[index]] = index;
  for (const std::string required : {"pair_id", "origin", "destination"}) {
    if (columns.find(required) == columns.end()) {
      throw std::runtime_error("OD CSV is missing required column: " + required);
    }
  }
  auto optional = [&](const std::vector<std::string> &fields, const std::string &name) {
    const auto found = columns.find(name);
    return found == columns.end() || found->second >= fields.size() ? std::string()
                                                                   : fields[found->second];
  };

  std::vector<OdPair> pairs;
  std::map<std::string, std::size_t> pair_ids;
  std::map<std::pair<int, int>, std::size_t> node_pairs;
  std::size_t line_number = 1;
  while (std::getline(input, line)) {
    ++line_number;
    if (line.empty()) continue;
    const auto fields = ParseCsvLine(line);
    try {
      const auto get = [&](const std::string &name) -> const std::string & {
        const std::size_t index = columns.at(name);
        if (index >= fields.size()) throw std::runtime_error("missing value for " + name);
        return fields[index];
      };
      OdPair pair{get("pair_id"), ParseInt(get("origin"), "origin"),
                  ParseInt(get("destination"), "destination"),
                  optional(fields, "origin_osm_id"), optional(fields, "destination_osm_id"),
                  optional(fields, "flight_distance_km"), optional(fields, "distance_band")};
      if (pair.pair_id.empty()) throw std::runtime_error("pair_id is empty");
      if (!pair_ids.emplace(pair.pair_id, line_number).second) {
        throw std::runtime_error("duplicate pair_id " + pair.pair_id);
      }
      if (!node_pairs.emplace(std::make_pair(pair.origin, pair.destination), line_number).second) {
        throw std::runtime_error("duplicate origin/destination pair");
      }
      pairs.push_back(std::move(pair));
    } catch (const std::exception &error) {
      throw std::runtime_error("invalid OD CSV line " + std::to_string(line_number) + ": " +
                               error.what());
    }
  }
  if (pairs.empty()) throw std::runtime_error("OD CSV contains no pairs");
  return pairs;
}

std::unordered_map<int, long long> LoadOsmNodeMap(const fs::path &path) {
  struct Record {
    int node_id;
    long long osm_node_id;
  };
  std::unordered_map<int, long long> mapping;
  H5::Exception::dontPrint();
  try {
    H5::H5File file(path.string(), H5F_ACC_RDONLY);
    if (H5Lexists(file.getId(), "/Index/OsmNodeMap", H5P_DEFAULT) <= 0) return mapping;
    H5::DataSet dataset = file.openDataSet("/Index/OsmNodeMap");
    const hsize_t count = dataset.getSpace().getSimpleExtentNpoints();
    std::vector<Record> records(count);
    H5::CompType type(sizeof(Record));
    type.insertMember("nodeId", HOFFSET(Record, node_id), H5::PredType::NATIVE_INT32);
    type.insertMember("osmNodeId", HOFFSET(Record, osm_node_id), H5::PredType::NATIVE_INT64);
    dataset.read(records.data(), type);
    for (const auto &record : records) mapping[record.node_id] = record.osm_node_id;
  } catch (const H5::Exception &error) {
    throw std::runtime_error("cannot read /Index/OsmNodeMap: " + error.getDetailMsg());
  }
  return mapping;
}

void ValidatePairs(const std::vector<OdPair> &pairs,
                   const std::shared_ptr<Routing::Data::GraphMemory> &graph) {
  std::vector<std::string> errors;
  for (const auto &pair : pairs) {
    if (pair.origin == pair.destination) errors.push_back(pair.pair_id + ": origin equals destination");
    if (!graph->ContainsNode(pair.origin)) errors.push_back(pair.pair_id + ": unknown origin " + std::to_string(pair.origin));
    if (!graph->ContainsNode(pair.destination)) errors.push_back(pair.pair_id + ": unknown destination " + std::to_string(pair.destination));
    if (errors.size() >= 10) break;
  }
  if (!errors.empty()) {
    std::ostringstream message;
    message << "OD validation failed";
    for (const auto &error : errors) message << "\n  " << error;
    throw std::runtime_error(message.str());
  }
}

QueryOutcome RunQuery(const Algorithm &algorithm, int origin, int destination,
                      unsigned int max_routes) {
  QueryOutcome outcome;
  const auto started = Clock::now();
  try {
    ScopedCoutSilencer silencer;
    outcome.results = algorithm.GetResults(origin, destination, max_routes, false);
  } catch (const std::exception &error) {
    outcome.error = error.what();
  } catch (...) {
    outcome.error = "unknown exception";
  }
  outcome.milliseconds = Milliseconds(Clock::now() - started);
  return outcome;
}

std::string OsmId(int routing_id, const std::unordered_map<int, long long> &mapping) {
  const auto found = mapping.find(routing_id);
  return found == mapping.end() ? std::string() : std::to_string(found->second);
}

void WriteRouteNodes(std::ostream &output, const std::string &profile, const OdPair &pair,
                     int repetition, std::size_t route_index, const Result &result,
                     const std::shared_ptr<Routing::Data::GraphMemory> &graph,
                     const std::unordered_map<int, long long> &mapping) {
  const auto &segments = result.GetResult();
  std::vector<int> nodes;
  nodes.reserve(segments.size() + 1);
  for (const auto &segment : segments) nodes.push_back(segment.nodeId1);
  if (!segments.empty()) nodes.push_back(segments.back().nodeId2);
  for (std::size_t node_index = 0; node_index < nodes.size(); ++node_index) {
    const auto &node = graph->GetNodeById(nodes[node_index]);
    output << CsvEscape(profile) << ',' << CsvEscape(pair.pair_id) << ',' << repetition << ','
           << route_index << ',' << node_index << ',' << nodes[node_index] << ','
           << OsmId(nodes[node_index], mapping) << ',' << node.GetLatitude() << ','
           << node.GetLongitude() << '\n';
  }
}

double Percentile(std::vector<double> values, double percentile) {
  if (values.empty()) return 0.0;
  std::sort(values.begin(), values.end());
  const double position = percentile * static_cast<double>(values.size() - 1);
  const auto lower = static_cast<std::size_t>(std::floor(position));
  const auto upper = static_cast<std::size_t>(std::ceil(position));
  const double fraction = position - static_cast<double>(lower);
  return values[lower] + (values[upper] - values[lower]) * fraction;
}

void WriteProfiles(const fs::path &path, const std::vector<ProfileRuntime> &profiles,
                   const Options &options) {
  std::ofstream output(path);
  if (!output) throw std::runtime_error("cannot create " + path.string());
  output << "profile,max_routes,similarity,all_filters_off,geometry,hierarchy,min_frc,max_frc,"
            "first_frc,second_frc,first_km,second_km,first_long_km,second_long_km,long_threshold_km\n";
  for (const auto &runtime : profiles) {
    const auto &profile = runtime.profile;
    const auto &settings = profile.settings.filterSettings;
    std::string geometry = "not-set";
    if (settings.typeOfFilter == Routing::Algorithms::GraphFilterGeometry::Ellipse) geometry = "ellipse";
    if (settings.typeOfFilter == Routing::Algorithms::GraphFilterGeometry::Circle) geometry = "circle";
    output << CsvEscape(profile.name) << ',' << options.max_routes.value_or(profile.max_routes) << ','
           << profile.settings.similarityPercent << ',' << settings.allFilterOff << ',' << geometry << ','
           << settings.hierarchyStartPositionFilterOn << ',' << settings.minFRC << ',' << settings.maxFRC << ','
           << settings.firstHierarchyJumpFRC << ',' << settings.secondHierarchyJumpFRC << ','
           << settings.firstHierarchyJumpDistance << ',' << settings.secondHierarchyJumpDistance << ','
           << settings.firstHierarchyJumpLongDistance << ',' << settings.secondHierarchyJumpLongDistance << ','
           << settings.longDistanceValue << '\n';
  }
}

int Run(const Options &options) {
  const auto process_started = Clock::now();
  if (!fs::exists(options.map_path)) throw std::runtime_error("map does not exist: " + options.map_path.string());
  fs::create_directories(options.output_directory);

  const auto graph_started = Clock::now();
  const auto graph = Routing::Tests::LoadGraph(options.map_path.string());
  const double graph_load_ms = Milliseconds(Clock::now() - graph_started);
  const auto osm_mapping = LoadOsmNodeMap(options.map_path);

  const auto od_started = Clock::now();
  const std::vector<OdPair> pairs = LoadOdPairs(options.od_path);
  ValidatePairs(pairs, graph);
  const double od_load_ms = Milliseconds(Clock::now() - od_started);

  std::vector<ProfileRuntime> profiles;
  for (auto profile : SelectProfiles(options)) {
    const auto setup_started = Clock::now();
    auto algorithm = std::make_unique<Algorithm>(graph, profile.settings);
    const double setup_ms = Milliseconds(Clock::now() - setup_started);
    profiles.push_back({std::move(profile), std::move(algorithm), setup_ms});
  }

  const fs::path queries_path = options.output_directory / "queries.csv";
  const fs::path routes_path = options.output_directory / "routes.csv";
  const fs::path nodes_path = options.output_directory / "route-nodes.csv";
  std::ofstream queries(queries_path);
  std::ofstream routes(routes_path);
  std::ofstream route_nodes(nodes_path);
  if (!queries || !routes || !route_nodes) throw std::runtime_error("cannot create output CSV files");
  queries << "phase,profile,repetition,pair_id,origin,destination,origin_osm_id,destination_osm_id,"
             "flight_distance_km,distance_band,query_ms,route_count,best_travel_time_s,best_length_m,status,error\n";
  routes << "profile,repetition,pair_id,route_index,travel_time_s,length_m,segment_count\n";
  route_nodes << "profile,pair_id,repetition,route_index,node_index,routing_node_id,osm_node_id,latitude,longitude\n";
  queries << std::fixed << std::setprecision(6);
  routes << std::fixed << std::setprecision(6);
  route_nodes << std::fixed << std::setprecision(6);

  struct RepetitionSummary {
    std::string profile;
    unsigned int repetition;
    std::size_t ok;
    std::size_t no_route;
    std::size_t errors;
    double query_total_ms;
    double wall_ms;
  };
  std::vector<RepetitionSummary> repetition_summaries;

  for (auto &runtime : profiles) {
    const unsigned int max_routes = options.max_routes.value_or(runtime.profile.max_routes);
    const unsigned int warmups = options.warmups;
    const unsigned int repetitions = options.repetitions;
    for (unsigned int phase_index = 0; phase_index < warmups + repetitions; ++phase_index) {
      const bool warmup = phase_index < warmups;
      const unsigned int repetition = warmup ? phase_index : phase_index - warmups;
      const auto repetition_started = Clock::now();
      double query_total_ms = 0.0;
      std::size_t ok = 0;
      std::size_t no_route = 0;
      std::size_t errors = 0;
      for (const auto &pair : pairs) {
        QueryOutcome outcome = RunQuery(*runtime.algorithm, pair.origin, pair.destination, max_routes);
        query_total_ms += outcome.milliseconds;
        if (!warmup) runtime.measured_times.push_back(outcome.milliseconds);
        const std::size_t route_count = outcome.results == nullptr ? 0 : outcome.results->size();
        std::string status = "ok";
        if (!outcome.error.empty()) {
          status = "error";
          ++errors;
        } else if (route_count == 0) {
          status = "no_route";
          ++no_route;
        } else {
          ++ok;
        }

        double best_time = 0.0;
        int best_length = 0;
        if (route_count > 0) {
          const auto best = std::min_element(
              outcome.results->begin(), outcome.results->end(),
              [](const Result &left, const Result &right) { return left.travelTime < right.travelTime; });
          best_time = best->travelTime;
          best_length = best->travelLength;
        }
        queries << (warmup ? "warmup" : "measured") << ',' << CsvEscape(runtime.profile.name) << ','
                << repetition << ',' << CsvEscape(pair.pair_id) << ',' << pair.origin << ',' << pair.destination
                << ',' << CsvEscape(pair.origin_osm_id) << ',' << CsvEscape(pair.destination_osm_id) << ','
                << CsvEscape(pair.flight_distance_km) << ',' << CsvEscape(pair.distance_band) << ','
                << outcome.milliseconds << ',' << route_count << ',';
        if (route_count > 0) queries << best_time << ',' << best_length;
        else queries << ',';
        queries << ',' << status << ',' << CsvEscape(outcome.error) << '\n';

        if (!warmup && outcome.results != nullptr) {
          for (std::size_t route_index = 0; route_index < outcome.results->size(); ++route_index) {
            const auto &route = outcome.results->at(route_index);
            routes << CsvEscape(runtime.profile.name) << ',' << repetition << ',' << CsvEscape(pair.pair_id)
                   << ',' << route_index << ',' << route.travelTime << ',' << route.travelLength << ','
                   << route.GetResult().size() << '\n';
            if (options.export_paths) {
              WriteRouteNodes(route_nodes, runtime.profile.name, pair, static_cast<int>(repetition), route_index,
                              route, graph, osm_mapping);
            }
          }
        }
      }
      const double wall_ms = Milliseconds(Clock::now() - repetition_started);
      if (!warmup) {
        runtime.ok += ok;
        runtime.no_route += no_route;
        runtime.errors += errors;
        repetition_summaries.push_back(
            {runtime.profile.name, repetition, ok, no_route, errors, query_total_ms, wall_ms});
      }
      std::cerr << (warmup ? "warmup" : "measured") << " profile=" << runtime.profile.name
                << " repetition=" << repetition << " queries=" << pairs.size() << " wall_ms=" << wall_ms
                << '\n';
    }
  }

  WriteProfiles(options.output_directory / "profiles.csv", profiles, options);
  std::ofstream summary(options.output_directory / "summary.csv");
  if (!summary) throw std::runtime_error("cannot create summary.csv");
  summary << "record_type,profile,repetition,queries,ok,no_route,errors,graph_load_ms,od_load_ms,"
             "profile_setup_ms,query_total_ms,wall_ms,min_ms,mean_ms,median_ms,p95_ms,max_ms\n";
  summary << std::fixed << std::setprecision(6);
  for (const auto &item : repetition_summaries) {
    summary << "repetition," << CsvEscape(item.profile) << ',' << item.repetition << ',' << pairs.size() << ','
            << item.ok << ',' << item.no_route << ',' << item.errors << ",,,," << item.query_total_ms << ','
            << item.wall_ms << ",,,,,\n";
  }
  for (const auto &runtime : profiles) {
    const auto &times = runtime.measured_times;
    const double total = std::accumulate(times.begin(), times.end(), 0.0);
    const double mean = times.empty() ? 0.0 : total / static_cast<double>(times.size());
    const auto bounds = std::minmax_element(times.begin(), times.end());
    summary << "profile," << CsvEscape(runtime.profile.name) << ",," << times.size() << ',' << runtime.ok << ','
            << runtime.no_route << ',' << runtime.errors << ",,," << runtime.setup_ms << ',' << total << ",,"
            << (times.empty() ? 0.0 : *bounds.first) << ',' << mean << ',' << Percentile(times, 0.5) << ','
            << Percentile(times, 0.95) << ',' << (times.empty() ? 0.0 : *bounds.second) << '\n';
  }
  const double process_wall_ms = Milliseconds(Clock::now() - process_started);
  summary << "process,,," << pairs.size() << ",,,," << graph_load_ms << ',' << od_load_ms << ",,,"
          << process_wall_ms << ",,,,,\n";

  std::cout << "Completed " << pairs.size() << " pair(s) with " << profiles.size() << " profile(s).\n"
            << "Graph load: " << graph_load_ms << " ms\n"
            << "Results: " << options.output_directory << '\n';
  if (osm_mapping.empty()) {
    std::cout << "Note: this legacy map has no /Index/OsmNodeMap; OSM ID columns are blank.\n";
  }
  return EXIT_SUCCESS;
}
}  // namespace

int main(int argc, char **argv) {
  try {
    return Run(ParseOptions(argc, argv));
  } catch (const std::exception &error) {
    std::cerr << "error: " << error.what() << "\nRun with --help for usage.\n";
    return EXIT_FAILURE;
  }
}
