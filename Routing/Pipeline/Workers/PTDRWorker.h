#pragma once

#include "RoutingPipelineWorkerBase.h"
#include "../PipelineConfig.h"
#include "../../Algorithms/Probability/Base/ProbabilityAlgorithm.h"
#include "../../Algorithms/Probability/Data/Segment.h"
#include "../../Algorithms/Probability/Data/ProbabilityResult.h"

namespace Routing {
    namespace Pipeline {

        class PTDRWorker : public PipelineWorkerBase {
        public:
            PTDRWorker(const PipelineConfig &pipelineConfig);

        private:
            void ProcessRequest(RoutingRequest &routingRequest) override;

            std::unique_ptr<Routing::Probability::ProbabilityAlgorithm> probabilityAlgorithm;

            std::vector<Routing::Probability::Segment>
            PrepareSegments(const std::vector<Routing::Algorithms::Segment> &route);
        };
    }
}


