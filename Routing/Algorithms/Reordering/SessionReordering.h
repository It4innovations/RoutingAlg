#pragma once

#include "ReorderingBase.h"

namespace Routing {
    namespace Algorithms {
        class SessionReordering : public ReorderingBase {
        public:
            SessionReordering(const shared_ptr<Data::GraphMemory> &routingGraph, const vector<float> &weights);

			vector<long long int> Reorder(const std::vector<ReorderInput> &reorderInput,
										  std::vector<long long> &lastRoute, const long long int currentTmw, const long long int tmwSize);

        private:

            /**
             * Copmute current load of segments normalized by their length
             * @param reorderInput
             * @return normalized load of given route
             */
            float NormalizeCapacity(const ReorderInput &reorderInput);


            /**
             * Compute normalized overlap between alternative routes and last sent route for session
             * @param reorderInput
             * @param lastRoute
             * @return
             */
            float RouteOverlap(const ReorderInput &reorderInput, const std::vector<long long> &lastRoute);


            // Weighting factors for individual reordering parts
            std::vector<float> weights;
        };
    }
}
