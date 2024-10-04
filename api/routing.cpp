#include "routing.hpp"

#include "../Routing/Tests/IOUtils.h"
#include "../Routing/Algorithms/Common/Result.h"
#include "../Routing/Algorithms/OneToOne/Dijkstra/Dijkstra.h"
#include "../Routing/Algorithms/Alternatives/AlternativesPlateauAlgorithm.h"
#include "../Routing/Algorithms/Probability/Algorithms/MonteCarloProbability.h"

#include "../Routing/Algorithms/Probability/Data/Segment.h"
#include "../Routing/Algorithms/Probability/Data/ProbabilityResult.h"
#include "../Routing/Algorithms/Reordering/Data/ReorderInput.h"
#include "../Routing/Algorithms/Reordering/SessionReordering.h"
#include "../Routing/Data/Probability/ProfileStorageHDF5.h"

#include <unordered_set>


using GraphWrapper = std::shared_ptr<Routing::Data::GraphMemory>;
using RoutingAlgorithm = Routing::Algorithms::StaticRoutingAlgorithm;
using AlternativesAlgorithm = Routing::Algorithms::AlternativesAlgorithm;
using RoutingSegment = Routing::Algorithms::Segment;

using ProbabilitySegment = Routing::Probability::Segment;
using ProbabilityResult = Routing::Probability::ProbabilityResult;
using ProbabilityProfileStorage = Routing::Data::Probability::ProfileStorageHDF5;

using ProbRoute = std::pair<std::vector<RoutingSegment>, ProbabilityResult>;

void *load_graph(const char *path) {
    try {
        return new GraphWrapper(Routing::Tests::LoadGraph(path));
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception at load_graph" << std::endl;
    }

    return nullptr;
}

void free_graph(void *graph) {
    delete static_cast<GraphWrapper *>(graph);
}

void *load_probability_profile_storage(const char *profile_path) {
    auto *pps = new ProbabilityProfileStorage(std::string(profile_path), true);
    return pps;
}

void free_probability_profile_storage(void *profile) {
    delete static_cast<ProbabilityProfileStorage *>(profile);
}

void set_segment_speeds(void *graph, SegmentSpeed *speeds, usize count) {
    auto ptr = *static_cast<GraphWrapper *>(graph);
    for (size_t i = 0; i < count; i++) {
        ptr->SetEdgeSpeed(speeds[i].id, speeds[i].speed);
    }
}

std::vector<RoutingSegment> find_path(RoutingAlgorithm& algorithm, u32 start_node, u32 end_node)
{
    auto result = algorithm.GetResult(start_node, end_node, Routing::CostCalcType::Len_cost);
    if (!result)
    {
        return std::vector<RoutingSegment>();
    }
    return result->GetResult();
}

Route dijkstra(void *graph, u32 origin, u32 destination) {
    Routing::Algorithms::Dijkstra algorithm(*static_cast<GraphWrapper *>(graph));
    auto route = find_path(algorithm, origin, destination);

    static thread_local std::vector<RouteSegment> segments;
    Route result{};
    result.count = route.size();

    if (result.count) {
        segments.resize(result.count);
        result.data = segments.data();
        for (size_t i = 0; i < result.count; i++) {
            result.data[i].start = static_cast<uint32_t>(route[i].nodeId1);
            result.data[i].end = static_cast<uint32_t>(route[i].nodeId2);
            result.data[i].edge = static_cast<uint32_t>(route[i].edgeId);
        }
    }

    return result;
}

std::vector<std::vector<RoutingSegment>>
get_alternative_paths(AlternativesAlgorithm &algorithm, u32 start_node, u32 end_node) {

    // TODO: there is a call of GetResult with a bigger setting
    std::unique_ptr<std::vector<Routing::Algorithms::Result>> results =
            algorithm.GetResults(start_node, end_node);

    if (!results) {
        return std::vector<std::vector<RoutingSegment>>();
    }

    std::vector<std::vector<RoutingSegment>> result_;
    for (auto &res : *results) {
        result_.push_back(res.GetResult());
    }
    return result_;
}

std::vector<ProbabilitySegment>
transform_to_probability_segments(std::vector<RoutingSegment> &route, ProbabilityProfileStorage &pps) {

    std::vector<ProbabilitySegment> prob_segments;
    std::transform(route.begin(), route.end(), std::back_inserter(prob_segments),
                   [&pps](auto &routing_segment) -> ProbabilitySegment {
                       return ProbabilitySegment(
                               routing_segment.edgeId,
                               routing_segment.length,
                               routing_segment.speed,
                               pps.GetProbabilityProfileByEdgeId(routing_segment.edgeId));
                   });
    return prob_segments;
}

struct PtdrMetadata {
    unsigned int routes_count;
    unsigned int samples_count;
    unsigned int start_epoch;
};


ProbabilityResult compute_probability_metadata(std::vector<RoutingSegment> &route,
                                               ProbabilityProfileStorage &pps,
                                               unsigned long start_time, /* NOTE: comes from global config */ /* TODO: remove and fix it to correct start time */
                                               unsigned int samples_count, /* route specific input */ /* TODO: what's the meaning? */
                                               unsigned int start_epoch /* route specific input */ /* TODO: why the epoch is of int type? */
) {

    std::vector<ProbabilitySegment> prob_route = transform_to_probability_segments(route, pps);

    auto input_departure = std::chrono::seconds(static_cast<long long>(start_epoch)); /* TODO: why static_cast? */

    /* SOLUTION: each car has to send time when they want to start; THIS IS WORKAROUND */
    float departure_time = input_departure.count() - start_time; /* TODO: why float data type? */
    if (departure_time < 0.0f) { // TODO: why? What's the meaning of this case?
        departure_time = start_time;
    }

    Routing::Probability::MonteCarloProbability prob_alg(samples_count,
                                                         1); // TODO: why is defaultProfileInterval set to 1?

    return prob_alg.GetProbabilityResult(prob_route, departure_time);
}

ARPiResult advance_routing_part1(void *graph, void *profile_storage,
                                 u32 origin, u32 destination) {

    GraphWrapper g = *static_cast<GraphWrapper *>(graph);

    // compute alternative routes
    Routing::Algorithms::AlternativesPlateauAlgorithm algorithm(g);
    auto routes = get_alternative_paths(algorithm, origin, destination);

    // prepare a set of edge ids for redis
    std::unordered_set<unsigned int> edge_ids;
    for (auto &route : routes) {
        for (auto &seg : route) {
            edge_ids.insert(seg.edgeId);
        }
    }
    unsigned int *edge_ids_ = new unsigned int[edge_ids.size()];
    std::copy(edge_ids.begin(), edge_ids.end(), edge_ids_);

    // perform PTDR on the routes
    std::vector<ProbRoute> *routes_with_prob_metadata = new std::vector<ProbRoute>();
    ProbabilityProfileStorage *pps = static_cast<ProbabilityProfileStorage *>(profile_storage);
    for (auto &route : routes) {
        routes_with_prob_metadata->emplace_back(
                std::move(route),
                compute_probability_metadata(route, *pps, 0, /* TODO: ? */ 10, /* TODO: ? */ 3));
    }
    routes.clear(); // clear invalided elements as they were moved

    return ARPiResult{edge_ids.size(), edge_ids_, routes_with_prob_metadata};
}

using Time_t = long long;
using Count_t = long long;

Route advance_routing_part2(
        void *routing_graph,
        void *ptdr_routes /* pointer to vector<ProbRoute> */,
        LoadOnSegments *current_load_on_segments /* pointer to map<segment id, map<time window, #cars>> */,
        Route old_route,
        u32 current_tmw, /* goes from redis */
        u32 tmw_size /* goes from config [default=60s]*/) {

    GraphWrapper routing_graph_ = *static_cast<GraphWrapper *>(routing_graph);
    std::vector<ProbRoute> ptdr_routes_ = *static_cast<std::vector<ProbRoute> *>(ptdr_routes);
    LoadOnSegments cls_ = *static_cast<LoadOnSegments *>(current_load_on_segments);

    std::vector<Routing::Algorithms::ReorderInput> reorder_input;
    for (const auto &ptdr_route : ptdr_routes_) {
      // preparing data
      std::vector<Routing::Algorithms::reorder_count> segments;
      segments.reserve(ptdr_route.first.size()); // TODO: change it from pair to struct!

      for (const auto &segment : ptdr_route.first) {
        int edge_id = segment.edgeId;
        auto end = cls_.load + cls_.count;
        auto load_on_segment = std::find_if(cls_.load, end, [edge_id](LoadOnSegment& segment_load) {
            return segment_load.segment_id == edge_id;
        });
        if (load_on_segment != end) {
          std::vector<std::pair<Time_t, Count_t>> load; // TODO: make it more readable
          std::transform(load_on_segment->load, load_on_segment->load + load_on_segment->count,
                         std::back_inserter(load),
                         [] (CountInTime& cit) {
                           return std::pair<Time_t, Count_t>(cit.time, cit.count);
                         });

          segments.emplace_back(segment, load);
        }
      }

      const ProbabilityResult &pbr = ptdr_route.second;
      reorder_input.emplace_back(Routing::Algorithms::ReorderInput(
        std::move(segments),
        pbr.computingTime,
        pbr.optimalTravelTime,
        pbr.minP,
        pbr.maxP,
        pbr.minTime,
        pbr.maxTime,
        pbr.mean,
        pbr.sampleDev));
    }

    Routing::Algorithms::SessionReordering reordering_alg(routing_graph_, { 1.0f, 1.0f, 1.0f });

    std::vector<long long int> last_route;
    for (size_t i = 0; i < old_route.count; i++)
    {
        last_route.push_back(old_route.data[i].edge);
    }
    auto reorder_result = reordering_alg.Reorder(reorder_input, last_route, current_tmw, tmw_size);

    std::vector<RoutingSegment> route;
    if (!reorder_result.empty())
    {
        int idx_of_best = reorder_result[0]; // NOTE: it returns a permutation sequence.
        // At the first position is the index to the best route.

        // take the best route
        route = ptdr_routes_[idx_of_best].first;
    }

    // prepare result
//    auto route = ptdr_routes_[0].first;

    Route result{};
    result.count = route.size();
    static thread_local std::vector<RouteSegment> segments;

    if (result.count)
    {
        segments.resize(result.count);
        result.data = segments.data();
        for (int i = 0; i < route.size(); i++)
        {
            result.data[i].start = static_cast<uint32_t>(route[i].nodeId1);
            result.data[i].end = static_cast<uint32_t>(route[i].nodeId2);
            result.data[i].edge = static_cast<uint32_t>(route[i].edgeId);

            auto edge = routing_graph_->GetEdgeById(route[i].edgeId);
            auto speed = edge.GetSpeed();
            auto length = edge.length;
            auto duration = length / (float) speed;
            result.data[i].duration = duration;
        }
    }

    return result;
}

void fill_segment_durations(void* graph, RouteSegment* segments, usize count)
{
    GraphWrapper routing_graph = *static_cast<GraphWrapper*>(graph);
    for (usize i = 0; i < count; i++)
    {
        auto edge = routing_graph->GetEdgeById(segments[i].edge);
        auto speed = edge.GetSpeed();
        auto length = edge.length;
        auto duration = length / (float) speed;
        segments[i].duration = duration;
    }
}
