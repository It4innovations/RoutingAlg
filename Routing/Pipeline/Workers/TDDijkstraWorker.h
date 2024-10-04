#pragma once


#include "RoutingPipelineWorkerBase.h"
#include "../PipelineConfig.h"
#include "../../Algorithms/TDAlgorithms/TDDijkstra.h"


namespace Routing {

    namespace Pipeline {

        class TDDijkstraWorker : public PipelineWorkerBase {

        public:
            TDDijkstraWorker(const PipelineConfig &config);


        private:
            void ProcessRequest(RoutingRequest &routingRequest) override;

            std::unique_ptr<Routing::Algorithms::TD::TDDijkstra> routingAlgorithm;
            int weekSeconds;

        };
    }
}



