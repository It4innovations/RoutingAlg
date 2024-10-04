
#include "TDDijkstraWorker.h"

Routing::Pipeline::TDDijkstraWorker::TDDijkstraWorker(const Routing::Pipeline::PipelineConfig &config)
        : PipelineWorkerBase(config) {

    LoadRoutingIndex();

    std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5>
            speedProf(new Routing::Data::Probability::ProfileStorageHDF5(pipelineConfig.speedProfilesPath,
                                                                         pipelineConfig.loadProfiles));

    this->weekSeconds = config.weekSeconds;

    this->routingAlgorithm.reset(new Routing::Algorithms::TD::TDDijkstra(routingIndex, speedProf));
}

void Routing::Pipeline::TDDijkstraWorker::ProcessRequest(Routing::Pipeline::RoutingRequest &routingRequest) {
    if (!(this->routingIndex->ContainsNode(routingRequest.origin) &&
          this->routingIndex->ContainsNode(routingRequest.destination))) {
        throw Exception::PipelineException(
                "Origin or destination node not found " + std::to_string(routingRequest.origin) + " - " +
                std::to_string(routingRequest.destination));
    }

    auto result = this->routingAlgorithm->GetResult(routingRequest.origin, routingRequest.destination,
                                                    this->weekSeconds);

    routingRequest.routes.push_back(result->GetResult());

    if (routingRequest.routes.back().size() < 1) {
        std::cerr << "Warning: empty route for request " << routingRequest.origin << " - " << routingRequest.destination
                  << std::endl;
    }

    std::cout << *result << std::endl;
}