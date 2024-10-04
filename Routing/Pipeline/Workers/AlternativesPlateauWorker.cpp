
#include "AlternativesPlateauWorker.h"

Routing::Pipeline::AlternativesPlateauWorker::AlternativesPlateauWorker(const Routing::Pipeline::PipelineConfig &config)
        : PipelineWorkerBase(config) {

    LoadRoutingIndex();

    this->weekSeconds = config.weekSeconds;

    this->algorithm = config.routingAlgorithm;

    alternativesCount = config.alternativeCount;

    switch (algorithm) {
        case PipelineConfig::RoutingAlgorithm::AlternativesPlateau:
            this->routingAlgorithm.reset(new Routing::Algorithms::AlternativesPlateauAlgorithm(this->routingIndex));
            break;

        case PipelineConfig::RoutingAlgorithm::AlternativesPlateauTD: {
            std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5>
                    speedProf(new Routing::Data::Probability::ProfileStorageHDF5(pipelineConfig.speedProfilesPath,
                                                                                 pipelineConfig.loadProfiles));
            Algorithms::AlgorithmSettings settingsAlternatives;
            settingsAlternatives.SetAlternativesTDPluginOn(true);
            this->routingAlgorithm.reset(
                    new Routing::Algorithms::AlternativesPlateauAlgorithm(this->routingIndex, speedProf,
                                                                          settingsAlternatives));
            break;
        }

        case PipelineConfig::RoutingAlgorithm::AlternativesPlateauProfileRecalculate: {
            std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5>
                    speedProf(new Routing::Data::Probability::ProfileStorageHDF5(pipelineConfig.speedProfilesPath,
                                                                                 pipelineConfig.loadProfiles));
            Algorithms::AlgorithmSettings settingsAlternatives;
            settingsAlternatives.SetAlternativesRouteTDRecalculateOn(true);
            this->routingAlgorithm.reset(
                    new Routing::Algorithms::AlternativesPlateauAlgorithm(this->routingIndex, speedProf,
                                                                          settingsAlternatives));
            break;
        }

        default:
            exit(1);
    }
}

void Routing::Pipeline::AlternativesPlateauWorker::ProcessRequest(Routing::Pipeline::RoutingRequest &routingRequest) {
    if (!(this->routingIndex->ContainsNode(routingRequest.origin) &&
          this->routingIndex->ContainsNode(routingRequest.destination))) {
        throw Exception::PipelineException(
                "Origin or destination node not found " + std::to_string(routingRequest.origin) +
                " - " + std::to_string(routingRequest.destination));
    }

    std::unique_ptr<std::vector<Result>> results;


    switch (this->algorithm) {
        case PipelineConfig::RoutingAlgorithm::AlternativesPlateau:
            results = this->routingAlgorithm->GetResults(routingRequest.origin, routingRequest.destination,
                                                         alternativesCount, true);
            break;

        case PipelineConfig::RoutingAlgorithm::AlternativesPlateauTD:
        case PipelineConfig::RoutingAlgorithm::AlternativesPlateauProfileRecalculate:
            results = this->routingAlgorithm->GetResults(routingRequest.origin, routingRequest.destination,
                                                         alternativesCount, true, weekSeconds);
            break;

        default:
            exit(1);
    }

    if (results == nullptr) {
        std::cerr << "Warning: empty route for request " << routingRequest.origin << " - " << routingRequest.destination
                  << std::endl;
        return;
    }

    int altCount = results->size();

    for (int i = 0; i < altCount; i++) {
        routingRequest.routes.push_back(results->at(i).GetResult());
    }


    for (int i = 0; i < altCount; i++) {
        std::cout << "Alt_" << i << "  " << results->at(i);
    }
}