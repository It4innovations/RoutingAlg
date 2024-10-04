#include "OneToOneWorker.h"

#include "../../Algorithms/OneToOne/Dijkstra/Dijkstra.h"
#include "../../Algorithms/OneToOne/Dijkstra/BidirectionalDijkstra.h"
#include "../../Algorithms/OneToOne/Astar/Astar.h"
#include "../../Algorithms/OneToOne/Astar/BidirectionalAstar.h"

#include <memory>

Routing::Pipeline::OneToOneWorker::OneToOneWorker(const Routing::Pipeline::PipelineConfig &config) :
    PipelineWorkerBase(config) {
    LoadRoutingIndex();

    switch (config.routingAlgorithm) {
        case PipelineConfig::RoutingAlgorithm::Dijkstra:
            this->routingAlgorithm = std::make_unique<Algorithms::Dijkstra>(this->routingIndex);
            break;
        case PipelineConfig::RoutingAlgorithm::BiDijkstra:
            this->routingAlgorithm = std::make_unique<Algorithms::BidirectionalDijkstra>(this->routingIndex);
            break;
        case PipelineConfig::RoutingAlgorithm::Astar:
            this->routingAlgorithm = std::make_unique<Algorithms::Astar>(this->routingIndex);
            break;
        case PipelineConfig::RoutingAlgorithm::BiAstar:
            this->routingAlgorithm = std::make_unique<Algorithms::BidirectionalAstar>(this->routingIndex);
            break;
        default:
            exit(1);
    }
}

void Routing::Pipeline::OneToOneWorker::ProcessRequest(Routing::Pipeline::RoutingRequest &routingRequest) {
    if (!this->routingIndex->ContainsNode(routingRequest.origin)) {
        throw Exception::NodeNotFoundException(routingRequest.origin);
    }

    if (!this->routingIndex->ContainsNode(routingRequest.destination)) {
        throw Exception::NodeNotFoundException(routingRequest.destination);
    }

    auto result = this->routingAlgorithm->GetResult(routingRequest.origin, routingRequest.destination,
                                                    CostCalcType::Len_cost);
    if (result) {
        routingRequest.routes.push_back(result->GetResult());
#ifndef NDEBUG
        std::cout << *result << std::endl;
#endif
    } else {
        routingRequest.routes.emplace_back();
    }

    if (routingRequest.routes.back().empty()) {
        std::cerr << "Warning: empty route for request " << routingRequest.origin << " - " << routingRequest.destination
                  << std::endl;
    }
}
