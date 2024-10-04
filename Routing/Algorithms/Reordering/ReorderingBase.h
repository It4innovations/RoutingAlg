#pragma once

#include "Data/ReorderInput.h"
#include "../../Data/Indexes/GraphMemory.h"

namespace Routing {
    namespace Algorithms {
        class ReorderingBase {
        public:
            ReorderingBase(const std::shared_ptr<Routing::Data::GraphMemory> &routingGraph);

			virtual vector<long long int> Reorder(const std::vector<ReorderInput> &reorderInput,
												  std::vector<long long> &lastRoute, const long long int currentTmw, const long long int tmwSize) = 0;

            //[<time_cat_0, <<gid1, pct>, <gid2, pct>>>, <time_cat_1, <<gid3, pct> ..>>]
            std::vector<std::pair<long long, std::vector<std::pair<long long, float>>>>
            QuantizeTravelTime(const std::vector<long long> &route, const int currentTmw, const int tmwSize);

        protected:


            std::shared_ptr<Routing::Data::GraphMemory> routingGraph;
        };
    }
}
