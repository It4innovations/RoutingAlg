#pragma once

#include <memory>
#include <vector>

#include "AlternativesAlgorithm.h"
#include "AlternativesPlateauTDPlugin.h"
#include "RouteSolution.h"
#include "../Common/AlgorithmHelper.h"
#include "../GraphFilter/GraphFilterGeometry.h"
#include "../../Data/Indexes/GraphMemory.h"
#include "../../Data/Utility/Definitions.h"

// used
#include "../../Data/Utility/Constants.h"

using Routing::Algorithms::AlternativesAlgorithm;
using Routing::Data::GraphMemory;

namespace Routing::Algorithms {
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
                                  GraphFilterGeometry plateauFilter, const google::dense_hash_map<int, float> &edgesSpeed,
                                  bool allFilterOff) const;

        void DijkstraBack(BinHeap &openSetBack, VisitedNodeHashMap &closedSetBack,
                                 GraphFilterGeometry plateauFilter, const google::dense_hash_map<int, float> &edgesSpeed,
                                 bool allFilterOff) const;

        std::vector<RouteSolution>
        FindPlateaus(VisitedNodeHashMap &closedSetForth, VisitedNodeHashMap &closedSetBack, int startId, int endId,
                     GraphFilterGeometry::FilterType typeOfFilter, bool multiThreading) const;

        void
        AddAlternativeSolution(VisitedNodeHashMap &closedSetForth, VisitedNodeHashMap &closedSetBack, int startId,
                               int endId,
                               int intersectionId, int plateauLength, float plateauTime,
                               std::vector<RouteSolution> &solutions, float &solutionTime, bool multiThreading) const;

        void ForthPartPlateau(VisitedNodeHashMap &closedSetForth, VisitedNodeHashMap &closedSetBack,
                              HashSet &processedNodeIds,
                              int actualId, int nextId, int &plateauLength, float &plateauTime) const;

        void BackPartPlateau(VisitedNodeHashMap &closedSetForth, VisitedNodeHashMap &closedSetBack,
                             HashSet &processedNodeIds,
                             int actualId, int previousId, int &plateauLength, float &plateauTime) const;

        GraphFilterGeometry
        InitiateFilters(Node startNode, Node endNode, Node startPositionNode, bool longDistance) const;

        void GetStartInfo(Node &startNode, Node &endNode, bool &longDistance) const;

        void RoutesRecalculateBySpeedProfile(std::vector<std::vector<Segment>> &results, int startTime) const;

        std::vector<std::vector<Segment>> AlternativesRecalculate(int startTime, unsigned int maxRoutes,
                                                                  std::vector<std::vector<Segment>> results) const;

        std::vector<std::vector<Segment>>
        AlternativesSimilarityParameter(std::vector<std::vector<Segment>> &results,
                                        double percentageSimilarity, int countResults) const;

        bool alternativesTDPluginOn;
        bool alternativesRouteTDRecalculateOn;
        bool useSimilarityParameter;

        unsigned int altCountsForRecalculation;
        int similarityParMultiplyConst;
        int similarityPercent;

        //google::dense_hash_map<int, float> edgesSpeed;

        const std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5> profileStorage;

    protected:
        std::vector<std::vector<Segment>> GetRoutes(int startId, int endId, unsigned int maxRoutes,
                                                    bool multiThreading, int startTime) const final;
    };
}

