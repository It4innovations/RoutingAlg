#include "ReorderingWorker.h"
#include "../../Algorithms/Reordering/SessionReordering.h"

Routing::Pipeline::ReorderingWorker::ReorderingWorker(const Routing::Pipeline::PipelineConfig &pipelineConfig)
        : PipelineWorkerBase(pipelineConfig) {
    // Initialize reordering alg with default weights
    this->reordering.reset(new Routing::Algorithms::SessionReordering(this->routingIndex, {1.0f, 1.0f, 1.0f}));
}

void Routing::Pipeline::ReorderingWorker::ProcessRequest(Routing::Pipeline::RoutingRequest &routingRequest) {
    // TODO
    //routingRequest.routeOrder = this->reordering->Reorder(this->RequestToInput(routingRequest), );
}