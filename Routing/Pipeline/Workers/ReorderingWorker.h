#pragma once

#include "RoutingPipelineWorkerBase.h"
#include "../PipelineConfig.h"
#include "../../Algorithms/Reordering/ReorderingBase.h"
#include "../../Algorithms/Reordering/Data/ReorderInput.h"


namespace Routing {
    namespace Pipeline {
        class ReorderingWorker : public PipelineWorkerBase {
        public:
            ReorderingWorker(const PipelineConfig &pipelineConfig);

        protected:
            void ProcessRequest(RoutingRequest &routingRequest) override;

        private:
            Routing::Algorithms::ReorderInput RequestToInput(const RoutingRequest &routingRequest);

            // Simulated session storage
            // Key: edge id, Value: sent route count
            // TODO: std::map<long, int>

            // Last route storage
            // Key: session id, Value: last route edgeId[]
            // TODO: std::map<long, std::vector<long>


            std::unique_ptr<Routing::Algorithms::ReorderingBase> reordering;

        };
    }
}




