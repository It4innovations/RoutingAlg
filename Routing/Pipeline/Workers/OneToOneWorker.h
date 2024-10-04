#pragma once

#include "RoutingPipelineWorkerBase.h"
#include "../PipelineConfig.h"
#include "../../Algorithms/Base/StaticRoutingAlgorithm.h"

namespace Routing {
    namespace Pipeline {
        class OneToOneWorker : public PipelineWorkerBase {
        public:
            OneToOneWorker(const PipelineConfig &config);


        private:
            void ProcessRequest(RoutingRequest &routingRequest) override;

            std::unique_ptr<Routing::Algorithms::StaticRoutingAlgorithm> routingAlgorithm;
        };
    }
}

