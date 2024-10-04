#pragma once

#include "RoutingPipelineWorkerBase.h"
#include "../PipelineConfig.h"
#include "../../Algorithms/Base/StaticRoutingAlgorithm.h"
#include "../../Algorithms/Alternatives/AlternativesPlateauAlgorithm.h"

namespace Routing {

    namespace Pipeline {

        class AlternativesPlateauWorker : public PipelineWorkerBase {

        public:
            AlternativesPlateauWorker(const PipelineConfig &config);


        private:
            void ProcessRequest(RoutingRequest &routingRequest) override;

            std::unique_ptr<Routing::Algorithms::AlternativesPlateauAlgorithm> routingAlgorithm;
            int alternativesCount = 5;
            int weekSeconds = 0;
            Routing::Pipeline::PipelineConfig::RoutingAlgorithm algorithm;
        };
    }
}



