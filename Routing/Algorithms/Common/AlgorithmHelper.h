#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include "Segment.h"
#include "DistanceInfo.h"
#include "VisitedNode.h"
#include "RouteInfo.h"
#include "../../Data/Indexes/GraphMemory.h"

#include <sparsehash/dense_hash_map>
#include <sparsehash/dense_hash_set>

using Routing::Data::GraphMemory;
using Routing::Algorithms::RouteInfo;

namespace Routing {

    namespace Algorithms {

        class AlgorithmHelper {
        public:

            static std::vector<Segment> GetRoadsNew(
                    google::dense_hash_map<int, VisitedNode> &closedSet,
                    const int startNodeId, const int endNodeId);

            static std::vector<Segment> GetRoadsReverseNew(
                    google::dense_hash_map<int, VisitedNode> &closedSet, const int startNodeId,
                    const int endNodeId, const int startLength, const float startTime);

            static std::vector<Segment> GetRoadsBidirectionalNew(
                    google::dense_hash_map<int, VisitedNode> &closedSetForth,
                    google::dense_hash_map<int, VisitedNode> &closedSetBack,
                    const int startNodeId, const int endNodeId, const int intersectionId);

            static std::vector<DistanceInfo> GetDistanceInfos(
                    google::dense_hash_map<int, VisitedNode> &closedSet, const int startNodeId,
                    const google::dense_hash_set<int> &endNodeIds);

            static RouteInfo
            GetRouteInfo(google::dense_hash_map<int, VisitedNode> &closedSet, const int startNodeId,
                         const int endNodeId);
        };
    }
}


