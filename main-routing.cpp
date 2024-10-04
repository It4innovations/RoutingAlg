#include <iostream>
#include <iomanip>
#include <getopt.h>
#include "Routing/Algorithms/Probability/Helpers/Utility.h"
#include "Routing/Pipeline/PipelineConfig.h"
#include "Routing/Pipeline/RoutingPipeline.h"
#include "Routing/Data/Utility/FilesystemUtil.h"
#include "Routing/Algorithms/OneToMany/DistanceMatrix.h"

using namespace std;
using namespace Routing;
using namespace Routing::Pipeline;
using namespace Routing::DistanceMatrix;

void usage() {
    cout << "Usage: routing-cli -c [config_file] -i [input_file] -o [output_file] [-x]" << endl;
}

int main(int argc, char **argv) {
    /**
     * Parse command line arguments
     */
    int ch;
    string configFile, inputFile, outputFile;
    bool distMatrix = false;

    while ((ch = getopt(argc, argv, "c:i:o:x")) != -1) {
        switch (ch) {
            case 'c':
                configFile = optarg;
                break;
            case 'i':
                inputFile = optarg;
                break;
            case 'o':
                outputFile = optarg;
                break;
            case 'x':
                distMatrix = true;
                break;
            case ':':
                cerr << "Missing argument." << endl;
                exit(EXIT_FAILURE);
            case '?':
            default:
                cerr << "Invalid command line parameters." << endl;
                usage();
                exit(EXIT_FAILURE);
        }
    }
    argc -= optind;
    argv += optind;


    try {
        /**
         * Load configuration
         */
        PipelineConfig pipelineConfig(configFile);
        cout << pipelineConfig << endl;

        /**
         * Distance matrix mode
         */
        if (distMatrix) {
            if (!Utils::CheckFileExists(inputFile)) {
                cerr << "ERROR: Input file: " << inputFile << " does not exist." << endl;
                return EXIT_FAILURE;
            }

            DistanceMatrix::ComputeDistanceMatrix(inputFile, outputFile, pipelineConfig);
            return EXIT_SUCCESS;
        }

        /**
         * Pipeline mode - Load input file
         */
        vector<RoutingRequest> requests = RoutingRequest::LoadRequests(
                inputFile,
                false);

        /**
         * Run pipeline
         */
        RoutingPipeline pipeline(pipelineConfig);
        for (auto &req : requests) {
            try {
                pipeline.ProcessRequest(req);
            }
            catch (const Exception::PipelineException &pe) {
                cerr << pe.what() << pe.msg << endl;
                continue;
            }
            catch (const Exception::NodeNotFoundException &ne) {
                cerr << ne.what() << ne.nodeId << endl;
                continue;
            }

            cout << req << endl;
        }
    }
    catch (const Exception::PipelineException &pe) {
        cerr << pe.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
