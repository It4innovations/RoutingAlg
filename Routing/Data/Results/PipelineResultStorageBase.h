#pragma once

#include "../../Pipeline/PipelineConfig.h"
#include "../../Pipeline/RoutingRequest.h"

namespace Routing {
    namespace Data {
        class PipelineResultStorageBase {
        public:
            PipelineResultStorageBase(const Pipeline::PipelineConfig &pipelineConfig) :
                    pipelineConfig(pipelineConfig) {};

            virtual ~PipelineResultStorageBase() {};

            virtual void StoreResult(const Pipeline::RoutingRequest &request) = 0;

        protected:
            Pipeline::PipelineConfig pipelineConfig;
        };
    }
}
