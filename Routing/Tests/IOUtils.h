#pragma once

#include <iostream>
#include <fstream>
#include <tuple>
#include <vector>
#include <iomanip>

#include "../Hdf/DataIndex.h"
#include "../Data/Indexes/GraphMemory.h"
#include "../Algorithms/Probability/Data/Data.h"
#include "../Algorithms/Common/Segment.h"
#include "IdPair.h"

using namespace Routing;
using namespace Routing::Data;
using namespace Routing::Hdf;
using namespace Routing::Probability;

namespace Routing {

    namespace Tests {

        std::vector<tuple<int, int>> LoadIdQuerySet(const string &queryFilePath);

        std::vector<GpsPoint> LoadGpsSet(const string &queryFilePath, int gpsOffset = 0);

        std::vector<IdPair> LoadIdGpsSet(const string &queryFilePath);

        std::vector<tuple<GpsPoint, GpsPoint>> LoadGpsQuerySet(const string &queryFilePath, int gpsOffset = 0);

        shared_ptr<GraphMemory> LoadGraph(const string &hdfIndexPath);

        tuple<int, int> ParseIdQuery(const string &queryString);

        GpsPoint ParseGps(const string &queryString, int gpsOffset = 0);

        tuple<GpsPoint, GpsPoint> ParseGpsQuery(const string &queryString, int gpsOffset = 0);

        vector<Routing::Probability::Segment>
        LoadProbabilitySegments(const string &edgesPath, const string &profilePath, int segmentCount);

        std::vector<IdPair>
        RemoveNonExistentNodes(std::vector<IdPair> &idPairs, shared_ptr<GraphMemory> routingGraph);

        vector<Routing::Algorithms::Segment> LoadResult(const string &resultPath);

        vector<int> LoadTestResult(const string &resultPath);

        Routing::Algorithms::Segment ParseResultFile(string line);

        int ParseTestLine(string line);
    }
}
