#include <H5Cpp.h>
#include "RoutingPipelineWorkerBase.h"


Routing::Pipeline::PipelineWorkerBase::PipelineWorkerBase(const Routing::Pipeline::PipelineConfig &pipelineConfig)
        : pipelineConfig(pipelineConfig) {

}

void Routing::Pipeline::PipelineWorkerBase::AddWorker(
        std::unique_ptr<Routing::Pipeline::PipelineWorkerBase> nextWorker) {
    if (this->nextWorker != nullptr) {
        this->nextWorker->AddWorker(std::move(nextWorker));
    } else {
        this->nextWorker = std::move(nextWorker);
    }
}

void
Routing::Pipeline::PipelineWorkerBase::SetNext(std::unique_ptr<Routing::Pipeline::PipelineWorkerBase> nextWorker) {
    this->nextWorker = std::move(nextWorker);
}

void Routing::Pipeline::PipelineWorkerBase::Process(Routing::Pipeline::RoutingRequest &routingRequest) {
    Utils::TimeWatch watch;
    watch.Start();
    this->ProcessRequest(routingRequest);
    watch.Stop();
    routingRequest.times.push_back(watch.GetElapsed());
#ifndef NDEBUG
    std::cout << "Processed in " << routingRequest.times.back() << " ms" << std::endl;
#endif

    if (this->nextWorker) {
        this->nextWorker->Process(routingRequest);
    }
}

void Routing::Pipeline::PipelineWorkerBase::LoadRoutingIndex() {
    H5::H5File h5file(this->pipelineConfig.routingIndexPath, H5F_ACC_RDONLY);
    unique_ptr<DataIndex> dif(new DataIndex(h5file));
    h5file.close();
    this->routingIndex.reset(new Data::GraphMemory(*dif)); // TODO: Refactor raw pointers
}

void Routing::Pipeline::PipelineWorkerBase::LoadSpeedProfiles() {
    this->speedProfiles.reset(
            new Data::Probability::ProfileStorageHDF5(pipelineConfig.speedProfilesPath, pipelineConfig.loadProfiles));

}
