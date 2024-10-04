#pragma once

#include <limits>
#include "../../Data/Enums/EVehiclesAccess.h"
#include "../GraphFilter/GraphFilterGeometry.h"
#include "FilterSettings.h"

namespace Routing {

    namespace Algorithms {

        class AlgorithmSettings {
        public:
            EVehiclesAccess vehicleAccess; //#REFACTORING# -- useless
            bool useTollWays;
            unsigned char maxVelocity;
            float vehicleWidth;
            float vehicleHeight;
            float vehicleWeight;
            float vehicleLength;
            float vehicleAxleLoad;
            FilterSettings filterSettings;

            void SetFilterType(GraphFilterGeometry::FilterType typeOfFilter) {
                this->filterSettings.typeOfFilter = typeOfFilter;
            }

            /***PART FOR ALTERNATIVES***/

            bool useSimilarityParameter;
            bool alternativesTDPluginOn;
            bool alternativesRouteTDRecalculateOn;

            int similarityPercent; // 0-100%
            int similarityParMultiplyConst;
            unsigned int altCountsForRecalculation;

            void SetUseSimilarityParameter(bool set){
                useSimilarityParameter = set;
            }

            void SetAlternativesTDPluginOn(bool set){
                alternativesTDPluginOn = set;
            }

            void SetAlternativesRouteTDRecalculateOn(bool set){
                alternativesRouteTDRecalculateOn = set;
            }

            void SetSimilarity(int similarityPercent){
                if(similarityPercent > 0 && similarityPercent < 100) {
                    this->similarityPercent = similarityPercent;
                }
            }

            void SetAltCountsForRecalculation(unsigned int count){
                altCountsForRecalculation = count;
            }

            AlgorithmSettings() :
                    vehicleAccess(EVehiclesAccess::Regular),
                    useTollWays(true),
                    maxVelocity(std::numeric_limits<unsigned char>::max()),
                    vehicleWidth(0),
                    vehicleHeight(0),
                    vehicleWeight(0),
                    vehicleLength(0),
                    vehicleAxleLoad(0),
                    useSimilarityParameter(false),
                    alternativesTDPluginOn(false),
                    alternativesRouteTDRecalculateOn(false),
                    similarityPercent(50),
                    similarityParMultiplyConst(5),
                    altCountsForRecalculation(20)
            {};
        };
    }
}


