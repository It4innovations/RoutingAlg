#include "PipelineResultStorageCSV.h"
#include <sstream>
#include <fstream>

Routing::Data::PipelineResultStorageCSV::PipelineResultStorageCSV(
        const Routing::Pipeline::PipelineConfig &pipelineConfig)
        : PipelineResultStorageBase(pipelineConfig) {}

void Routing::Data::PipelineResultStorageCSV::StoreResult(const Routing::Pipeline::RoutingRequest &request) {
    for (size_t i = 0; i < request.routes.size(); ++i) {
        // Routes
        std::stringstream rfileName("");
        rfileName << "route_" << i << "_" << request.origin << "_" << request.destination << ".csv";

        std::ofstream routeFile(rfileName.str());
        for (const auto &seg : request.routes[i]) {
            if (!routeFile.good()) {
                std::cerr << "Cannot write output file: " << rfileName.str() << std::endl;
            }

            routeFile << seg.edgeId << std::endl;
        }
        routeFile.close();

        // Probability
        if (pipelineConfig.ptdr && request.probabilityResult.size() > 0) {
            std::stringstream pfileName("");
            pfileName << "times_" << i << "_" << request.origin << "_" << request.destination << ".csv";
            routeFile.open(pfileName.str());
            for (const auto &pr : request.probabilityResult[i].times) {
                if (!routeFile.good()) {
                    std::cerr << "Cannot write output file: " << pfileName.str() << std::endl;
                }

                routeFile << pr.first << CSV_SEPARATOR << pr.second << std::endl;
            }
            routeFile.close();
        }
    }

}
