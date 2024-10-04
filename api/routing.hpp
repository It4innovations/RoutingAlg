#pragma once

using u32 = unsigned int;
using usize = unsigned long long;

struct RouteSegment {
    u32 start;
    u32 end;
    u32 edge;
    float duration;
};

struct Route {
    usize count;
    RouteSegment* data;
};

struct SegmentSpeed {
    u32 id;
    float speed;
};

struct ARPiResult { // Advance Routing Part (1)
  u32 edges_count;
  u32* edge_set;

  void* ptdr_routes;
};

struct CountInTime { // pair<time, count>
  u32 time;
  u32 count;
};

struct LoadOnSegment { // pair<segment_id, vector<CountInTime>>
  u32 segment_id;
  usize count;
  CountInTime* load;
};

struct LoadOnSegments { // vector<LoadOnSegment>
  usize count;
  LoadOnSegment* load;
};

extern "C" {
    void* load_graph(const char* path);
    void free_graph(void* graph);

    void* load_probability_profile_storage(const char* profile_path);
    void free_probability_profile_storage(void* profile_storage);

    void set_segment_speeds(void* graph, SegmentSpeed* speeds, usize count);

    Route dijkstra(void* graph, u32 origin, u32 destination);

    ARPiResult advance_routing_part1(void *graph, void* profile_path,
            u32 origin, u32 destination);

    Route advance_routing_part2(void *graph, void* ptdr_routes, LoadOnSegments *current_load_on_segments,
                                Route old_route, u32 current_tmw, u32 tmw_size);

    /**
     * Fills durations from the given graph (GraphWrapper) into the provided array of segments.
     */
    void fill_segment_durations(void* graph, RouteSegment* segments, usize count);
}
