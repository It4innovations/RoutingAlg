#pragma once

#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "AlternativesAlgorithm.h"
#include "AlternativesPlateauTDPlugin.h"
#include "RouteSolution.h"
#include "../Common/AlgorithmHelper.h"
#include "../Common/VisitedNode.h"
#include "../Common/RouteInfo.h"
#include "../GraphFilter/GraphFilterBase.h"
#include "../GraphFilter/GraphFilterGeometry.h"
#include "../GraphFilter/GraphFilterGeometryStartLocation.h"
#include "../GraphFilter/GraphFilterHierarchy.h"
#include "../../Data/Collections/BinaryHeap.h"
#include "../../Data/Indexes/GraphMemory.h"
#include "../../Data/Utility/Constants.h"
#include "../../Data/Utility/GPSDistanceCalculation.h"
#include "../../Data/Utility/Definitions.h"

using Routing::Algorithms::AlternativesAlgorithm;
using Routing::Data::GraphMemory;

namespace Routing {

    namespace Algorithms {
        class AlternativesPlateauAlgorithm : public AlternativesAlgorithm {

        public:
            explicit AlternativesPlateauAlgorithm(std::shared_ptr<GraphMemory> routingGraph,
                                                  AlgorithmSettings settings = AlgorithmSettings(),
                                                  TravelCostCalculator *travelCostCalculator = &TravelCostCalculator::Instance(),
                                                  TravelTimeCalculator *travelTimeCalculator = &TravelTimeCalculator::Instance());

            AlternativesPlateauAlgorithm(std::shared_ptr<GraphMemory> routingGraph,
                                         std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5> storage,
                                         AlgorithmSettings settings = AlgorithmSettings(),
                                         TravelCostCalculator *travelCostCalculator = &TravelCostCalculator::Instance(),
                                         TravelTimeCalculator *travelTimeCalculator = &TravelTimeCalculator::Instance());

            void UpdateAlternativesSettings(){
                alternativesTDPluginOn = this->settings.alternativesTDPluginOn;
                alternativesRouteTDRecalculateOn = this->settings.alternativesRouteTDRecalculateOn;
                useSimilarityParameter = this->settings.useSimilarityParameter;
                altCountsForRecalculation = this->settings.altCountsForRecalculation;
                similarityParMultiplyConst = this->settings.similarityParMultiplyConst;
                similarityPercent = this->settings.similarityPercent;
            }

        private:
            void DijkstraForth(BinHeap &openSetForth, VisitedNodeHashMap &closedSetForth,
                                      GraphFilterGeometry plateauFilter, bool allFilterOff);

            void DijkstraBack(BinHeap &openSetBack, VisitedNodeHashMap &closedSetBack,
                                     GraphFilterGeometry plateauFilter, bool allFilterOff);

            std::vector<RouteSolution>
            FindPlateaus(VisitedNodeHashMap &closedSetForth, VisitedNodeHashMap &closedSetBack, int startId, int endId,
                         GraphFilterGeometry::FilterType typeOfFilter, bool multiThreading);

            void
            AddAlternativeSolution(VisitedNodeHashMap &closedSetForth, VisitedNodeHashMap &closedSetBack, int startId,
                                   int endId,
                                   int intersectionId, int plateauLength, float plateauTime,
                                   std::vector<RouteSolution> &solutions, float &solutionTime, bool multiThreading);

            void ForthPartPlateau(VisitedNodeHashMap &closedSetForth, VisitedNodeHashMap &closedSetBack,
                                  HashSet &processedNodeIds,
                                  int actualId, int nextId, int &plateauLength, float &plateauTime);

            void BackPartPlateau(VisitedNodeHashMap &closedSetForth, VisitedNodeHashMap &closedSetBack,
                                 HashSet &processedNodeIds,
                                 int actualId, int previousId, int &plateauLength, float &plateauTime);

            GraphFilterGeometry
            InitiateFilters(Node startNode, Node endNode, Node startPositionNode, bool longDistance);

            void GetStartInfo(Node &startNode, Node &endNode, bool &longDistance);

            void RoutesRecalculateBySpeedProfile(std::vector<std::vector<Segment>> &results, int startTime);

            std::vector<std::vector<Segment>> AlternativesRecalculate(int startTime, unsigned int maxRoutes,
                                                                      std::vector<std::vector<Segment>> results);

            std::vector<std::vector<Segment>>
            AlternativesSimilarityParameter(std::vector<std::vector<Segment>> &results,
                                            double percentageSimilarity, int countResutls);

            bool alternativesTDPluginOn;
            bool alternativesRouteTDRecalculateOn;
            bool useSimilarityParameter;

            unsigned int altCountsForRecalculation;
            int similarityParMultiplyConst;
            int similarityPercent;

            google::dense_hash_map<int, float> edgesSpeed;

            std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5> profileStorage;

        protected:
            std::vector<std::vector<Segment>> GetRoutes(int startId, int endId, unsigned int maxRoutes = 3,
                                                        bool multiThreading = true, int startTime = 0) final;
        };
    }
}

