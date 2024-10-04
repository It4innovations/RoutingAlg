#pragma once

#include "../Algorithms/Common/Segment.h"
#include "../Algorithms/Probability/Data/ProbabilityResult.h"
#include "../Algorithms/Reordering/Data/ReorderInput.h"

#define CSV_SEPARATOR ';'

namespace Routing {
    namespace Pipeline {
        /**
         * RoutingRequest
         * Represents single routing request, which is passed through the routing pipeline.
         */
        struct RoutingRequest {
            /**
             * RoutingRequest
             * Request is defined by its input data
             * @param id - unique identifier of the request (row index in the input file)
             * @param origin
             * @param destination
             */
            RoutingRequest(int id, unsigned long origin, unsigned long destination) : id(id), origin(origin),
                                                                                      destination(destination) {}

            /**
             * LoadRequests
             * Loads requests from input CSV file
             * Format: origin;destination;departure_time;...
             * @param file
             * @param departureTimes
             * @return Vector of parsed requests
             */
            static std::vector<RoutingRequest> LoadRequests(const std::string file, int departureTime);

            int id;

            // Request data
            unsigned long origin;
            unsigned long destination;

            // Response data
            std::vector<std::vector<Routing::Algorithms::Segment>> routes;
            std::vector<Probability::ProbabilityResult> probabilityResult;
            std::vector<long> times;
            std::vector<int> routeOrder;

            friend std::ostream &operator<<(std::ostream &os, const RoutingRequest &res) {

                os << "-- Request --" << std::endl;
                os << "Origin: " << res.origin << std::endl;
                os << "Destination: " << res.destination << std::endl << std::endl;

                os << "-- Routing --" << std::endl;
                for (size_t i = 0; i < res.routes.size(); ++i) {
                    os << "Route " << i << ": " << res.routes[i].size() << " segments" << std::endl;
                    // TODO: Travel distance, travel time with limit speed

                    if (i < res.probabilityResult.size()) {
                        os << "-- PTDR Result --" << std::endl;
                        os << res.probabilityResult[i] << std::endl;
                    }
                }
                return os;
            }

        };


    }
}


