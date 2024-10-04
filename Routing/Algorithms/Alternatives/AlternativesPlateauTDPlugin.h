#pragma once

#define  SECONDSINWEEK 604800

#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "AlternativesAlgorithm.h"
#include "../GraphFilter/GraphFilterGeometry.h"
#include "../GraphFilter/GraphFilterGeometryStartLocation.h"
#include "../GraphFilter/GraphFilterBase.h"
#include "../GraphFilter/GraphFilterHierarchy.h"
#include "../TDAlgorithms/TDUtility.h"
#include "../../Data/Collections/BinaryHeap.h"
#include "../../Data/Collections/BinaryHeap.h"
#include "../../Data/Indexes/GraphMemory.h"
#include "../../Data/Probability/ProfileStorageHDF5.h"
#include "../../Data/Utility/Definitions.h"
#include "../../Data/Utility/GPSDistanceCalculation.h"


namespace Routing {

    namespace Algorithms {

        class AlternativesPlateauTDPlugin : public Algorithm {

        public:

            AlternativesPlateauTDPlugin(std::shared_ptr<GraphMemory> routingGraph,
                                        std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5> storage,
                                        AlgorithmSettings settings = AlgorithmSettings())
                    : Algorithm(std::move(routingGraph), settings) {

                this->storage = move(storage);
                this->oneSpeedIntervalInSeconds = this->storage->GetIntervalLength();
            };

            google::dense_hash_map<int, float>
            GetEdgesSpeedValue(int startId, int endId, GraphFilterGeometry plateauFilter,
                               bool allFilterOff, int startTime);

        protected:

            google::dense_hash_map<int, float> DijkstraForth(BinHeap &openSetForth, VisitedNodeHashMap &closedSetForth,
                                                             GraphFilterGeometry plateauFilter, bool allFilterOff);

            const int secondsInWeek = SECONDSINWEEK;

            int oneSpeedIntervalInSeconds = 0;

            std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5> storage;

        };

    }
}


