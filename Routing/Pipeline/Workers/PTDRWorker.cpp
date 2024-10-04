#include "PTDRWorker.h"
#include "../../Algorithms/Probability/Algorithms/MonteCarloProbability.h"

Routing::Pipeline::PTDRWorker::PTDRWorker(const Routing::Pipeline::PipelineConfig &pipelineConfig) : PipelineWorkerBase(
        pipelineConfig) {
    LoadSpeedProfiles();
    this->probabilityAlgorithm.reset(new Routing::Probability::MonteCarloProbability(this->pipelineConfig.samples,
                                                                                     this->speedProfiles->GetIntervalLength() *
                                                                                     this->speedProfiles->GetIntervalCount()));
}

void Routing::Pipeline::PTDRWorker::ProcessRequest(Routing::Pipeline::RoutingRequest &routingRequest) {

    for (auto &route : routingRequest.routes) {
#ifndef NDEBUG
        std::cout << "Running PTDR" << std::endl;
#endif
        std::vector<Routing::Probability::Segment> probabilitySegments = this->PrepareSegments(route);
        routingRequest.probabilityResult.push_back(this->probabilityAlgorithm->GetProbabilityResult(probabilitySegments,
                                                                                                    static_cast<float>(pipelineConfig.weekSeconds)));

        if (routingRequest.probabilityResult.back().times.size() < 1) {
            std::cerr << "Warning: empty PTDR result for request " << routingRequest.origin << " - "
                      << routingRequest.destination << std::endl;
        }
    }
}

std::vector<Routing::Probability::Segment>
Routing::Pipeline::PTDRWorker::PrepareSegments(const std::vector<Routing::Algorithms::Segment> &route) {
    std::vector<Routing::Probability::Segment> probabilitySegments;
    if (this->speedProfiles->GetSegmentCount() < 1) {
        return probabilitySegments;
    }

#ifndef NDEBUG
    std::cout << "Preparing " << route.size() << " segments." << std::endl;
#endif
    int travelledDistance = 0;
    float totalTime = 0.0f;
    for (const auto &seg : route) {
        int actualSegmentLength = seg.length - travelledDistance;
        travelledDistance = seg.length;

        float timeSpentOnSegment = seg.time - totalTime;
        int maxSpeed = actualSegmentLength / timeSpentOnSegment;
        totalTime = seg.time;

        probabilitySegments.emplace_back(seg.edgeId, actualSegmentLength, maxSpeed,
                                         this->speedProfiles->GetProbabilityProfileByEdgeId(seg.edgeId));
#ifndef NDEBUG
        std::cout << ((probabilitySegments.back().profile == nullptr) ? "-" : "0") << std::flush;
#endif
    }
#ifndef NDEBUG
    std::cout << std::endl;
#endif
    return probabilitySegments;
}
