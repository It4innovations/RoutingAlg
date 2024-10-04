#pragma once

#include "PipelineResultStorageBase.h"

namespace Routing {
    namespace Data {

        class PipelineResultStorageCSV : public PipelineResultStorageBase {
        public:
            PipelineResultStorageCSV(const Pipeline::PipelineConfig &pipelineConfig);

            virtual ~PipelineResultStorageCSV() {};

            void StoreResult(const Pipeline::RoutingRequest &request) override;


        };
    }
}


