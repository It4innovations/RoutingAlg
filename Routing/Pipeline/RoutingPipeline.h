#pragma once

#include <memory>
#include "PipelineConfig.h"
#include "Workers/RoutingPipelineWorkerBase.h"
#include "../Data/Results/PipelineResultStorageBase.h"


namespace Routing {
    namespace Pipeline {
        class RoutingPipeline {
        public:
            RoutingPipeline(const PipelineConfig &config);

            void ProcessRequest(RoutingRequest &request);

        private:
            PipelineConfig config;
            std::unique_ptr<PipelineWorkerBase> baseWorker;
            std::vector<std::unique_ptr<Routing::Data::PipelineResultStorageBase>> resultStorage;
        };
    }
}

