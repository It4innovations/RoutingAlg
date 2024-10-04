#pragma once

#include "../Data/Exceptions/Exceptions.h"
#include "../Algorithms/Probability/Helpers/Utility.h"

#include <string>
#include <iostream>

namespace Routing {
    namespace Pipeline {
        struct PipelineConfig {
            PipelineConfig(std::string configFile);

            std::string CheckConfig();

            enum class RoutingAlgorithm {
                Dijkstra = 1,
                BiDijkstra = 2,
                Astar = 3,
                BiAstar = 4,
                TDDijkstra = 5,
                AlternativesPlateau = 6,
                AlternativesPlateauTD = 7,
                AlternativesPlateauProfileRecalculate = 8
            };

            enum class ReorderingType {
                Average = 1,
                Median = 2

            };

            // Routing stage
            RoutingAlgorithm routingAlgorithm;
            int alternativeCount;
            std::string routingIndexPath;

            // PTDR stage
            bool ptdr;
            bool loadProfiles;
            int samples;
            std::string speedProfilesPath;
            short day, hour, minute; // Departure time
            int weekSeconds; // Dep. time in seconds in one week


            // Reordering stage
            ReorderingType reorderingType;

            // Config file
            std::string configFile;
            bool writeResultsCSV;
            bool writeResultsSQLite;

            friend std::ostream &operator<<(std::ostream &os, const PipelineConfig &conf) {
                {
                    os << "Routing pipeline configuration" << std::endl;
                    os << "[Config file: " << conf.configFile << "]\n" << std::endl;
                    os << "Write results CSV:\t\t" << conf.writeResultsCSV << std::endl;
                    os << "Write results SQLite:\t\t" << conf.writeResultsSQLite << std::endl;
                    os << "Routing index:\t\t" << conf.routingIndexPath << std::endl;
                    os << "Speed profiles:\t\t" << conf.speedProfilesPath << std::endl;
                    os << "Routing alg.:\t\t" << std::to_string(static_cast<int>(conf.routingAlgorithm)) << std::endl;
                    os << "Alt. count:\t\t\t" << conf.alternativeCount << std::endl;
                    os << "PTDR:\t\t\t\t" << conf.ptdr << std::endl;
                    os << "PTDR samples:\t\t" << conf.samples << std::endl;
                    os << "Load profiles:\t\t" << conf.loadProfiles << std::endl;
                    os << "Departure time:\t\t" << Routing::Probability::Utility::GetReadableTime(conf.weekSeconds)
                       << " ("
                       << conf.weekSeconds << " s)" << std::endl;
                    os << "Reordering:\t\t\t" << std::to_string(static_cast<int>(conf.reorderingType)) << std::endl;
                }
                return os;
            };
        };
    }
}


