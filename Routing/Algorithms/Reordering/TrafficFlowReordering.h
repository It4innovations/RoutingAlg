#pragma once

#include <map>
#include <vector>
#include <mutex>

#include "ReorderingBase.h"

namespace Routing {
    namespace Algorithms {

        // Mutex for locking the pheromone storage in TrafficFlowReordering
        std::mutex trafficReoLock;

        class TrafficFlowReordering : public ReorderingBase {
        public:
            TrafficFlowReordering(const shared_ptr<Data::GraphMemory> &routingGraph, const float beta,
                                  const float gamma);

            vector<long long int>
            Reorder(const std::vector<ReorderInput> &reorderInput, std::vector<std::pair<long long,
                    std::vector<long long>>> &lastRoute, const long long int currentTmw,
                    const long long int tmwSize) override;


        private:

            // ACO pheromone storage
            // Map Key: segment ID
            // Value: Map
            //            Key: time window
            //            Value: Pair:
            //                   First: pheromone value
            //                   Second: last update time
            std::map<long, std::map<long, std::pair<float, long>> pheromone;

        };
    }
}

