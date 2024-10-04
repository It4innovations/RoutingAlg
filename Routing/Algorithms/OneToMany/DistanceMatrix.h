#pragma once
#include <string>
#include "../../Pipeline/PipelineConfig.h"

using namespace std;

namespace Routing {
    namespace DistanceMatrix {
        void ComputeDistanceMatrix(const string &inputFile, const string &outputFile, const Pipeline::PipelineConfig &config);
    }
}