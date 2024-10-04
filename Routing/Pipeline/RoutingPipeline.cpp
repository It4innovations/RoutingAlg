#include <memory>

#include "RoutingPipeline.h"
#include "Workers/OneToOneWorker.h"
#include "Workers/PTDRWorker.h"
#include "Workers/AlternativesPlateauWorker.h"
#include "Workers/TDDijkstraWorker.h"
#include "../Data/Results/PipelineResultStorageSQLite.h"
#include "../Data/Results/PipelineResultStorageCSV.h"

Routing::Pipeline::RoutingPipeline::RoutingPipeline(const Routing::Pipeline::PipelineConfig &config) : config(config) {

    // Routing stage
    switch (config.routingAlgorithm) {
        case Routing::Pipeline::PipelineConfig::RoutingAlgorithm::Dijkstra:
        case Routing::Pipeline::PipelineConfig::RoutingAlgorithm::BiDijkstra:
        case Routing::Pipeline::PipelineConfig::RoutingAlgorithm::Astar:
        case Routing::Pipeline::PipelineConfig::RoutingAlgorithm::BiAstar:
            this->baseWorker = std::make_unique<OneToOneWorker>(config);
            break;
        case Routing::Pipeline::PipelineConfig::RoutingAlgorithm::TDDijkstra:
            this->baseWorker = std::make_unique<TDDijkstraWorker>(config);
            break;
        case Routing::Pipeline::PipelineConfig::RoutingAlgorithm::AlternativesPlateau:
        case Routing::Pipeline::PipelineConfig::RoutingAlgorithm::AlternativesPlateauTD:
        case Routing::Pipeline::PipelineConfig::RoutingAlgorithm::AlternativesPlateauProfileRecalculate:
            this->baseWorker = std::make_unique<AlternativesPlateauWorker>(config);
            break;
        default:
            std::cout << "Bad number for algorithm" << std::endl;
            break;
    }

    // PTDR stage
    if (config.ptdr) {
        this->baseWorker->AddWorker(std::unique_ptr<PipelineWorkerBase>(new PTDRWorker(config)));
    }

    // Result storage
    if (config.writeResultsCSV) {
        this->resultStorage.emplace_back(new Data::PipelineResultStorageCSV(this->config));
    }

    if (config.writeResultsSQLite) {
        this->resultStorage.emplace_back(new Data::PipelineResultStorageSQLite(this->config, "routing-result.sql"));
    }
}

void Routing::Pipeline::RoutingPipeline::ProcessRequest(Routing::Pipeline::RoutingRequest &request) {
    this->baseWorker->Process(request);

    for (const auto &st : this->resultStorage) {
        st->StoreResult(request);
    }
}
