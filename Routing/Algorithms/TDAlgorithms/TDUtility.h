#pragma once

#define  SECONDSINWEEK 604800

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include "../../Data/Structures/Edge.h"
#include "../../Data/Probability/ProfileStorageHDF5.h"
#include "../../Data/Indexes/GraphMemory.h"
#include "../Common/Segment.h"

namespace Routing {

    namespace Algorithms {

        namespace TD {

            float CalculateCostSecPrecise(const Edge &edge, float startTime,
                                          std::shared_ptr<std::vector<float>> edgeSpeedProfile,
                                          std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5> storage);

            float CalculateCostSec(const Edge &edge, float speedKmPH);

            void CalculateRoadCostByProfile(std::vector<Segment> &route, int startTime,
                                            const std::shared_ptr<Routing::Data::GraphMemory>& routingGraph,
                                            const std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5>& storage);

            std::vector<int> GetNumbersFromFile(const std::string& filePath);
        }
    }
}


