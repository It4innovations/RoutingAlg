#pragma once

#include <memory>
#include "../PipelineConfig.h"
#include "../RoutingRequest.h"
#include "../../Data/Indexes/GraphMemory.h"
#include "../../Data/Probability/ProfileStorageHDF5.h"

namespace Routing {
    namespace Pipeline {
        class PipelineWorkerBase {
        public:
            PipelineWorkerBase(const PipelineConfig &pipelineConfig);

            void Process(RoutingRequest &routingRequest);

            void AddWorker(std::unique_ptr<PipelineWorkerBase> nextWorker);

            void SetNext(std::unique_ptr<PipelineWorkerBase> nextWorker);

            virtual ~PipelineWorkerBase() {};

        protected:
            void LoadRoutingIndex();

            void LoadSpeedProfiles();

            virtual void ProcessRequest(RoutingRequest &routingRequest) = 0;

            PipelineConfig pipelineConfig;
            std::shared_ptr<Routing::Data::GraphMemory> routingIndex;
            std::shared_ptr<Routing::Data::Probability::ProfileStorageBase> speedProfiles;


        private:
            std::unique_ptr<PipelineWorkerBase> nextWorker;

        };
    }
}


