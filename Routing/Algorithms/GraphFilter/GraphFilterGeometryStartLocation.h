#pragma once

#include <cmath>
#include "../../Data/Structures/Node.h"
#include "GraphFilterBase.h"
#include "GraphFilterHierarchy.h"
#include "../../Data/Utility/GPSDistanceCalculation.h"

using Routing::Node;

namespace Routing {

    namespace Algorithms {

        class GraphFilterGeometryStartLocation : public GraphFilterBase {

        public:
            GraphFilterGeometryStartLocation(Node &startNode, double firstDistanceKm, double secondDistanceKm,
                                             int firstMinFRC, int secondMinFRC,
                                             GraphFilterHierarchy *graphFilterHierarchy) {
                this->startNode = startNode;
                this->distanceKm = firstDistanceKm;
                this->secondDistanceKm = secondDistanceKm;
                this->graphFilterHierarchy = graphFilterHierarchy;
                this->firstMinFRC = firstMinFRC;
                this->secondMinFRC = secondMinFRC;
                this->levelOfDistance = 1;
            }

            void DeleteAllFilter() final {
                if (nextFilter != nullptr) {
                    nextFilter->DeleteAllFilter();
                }

                delete nextFilter;
            }

            void Handle(bool &filtersPass) final {
                if (ApplyFilter(destinationNode)) {
                    return;
                }

                if (this->nextFilter != nullptr) {
                    nextFilter->Handle(filtersPass);
                }
            }

            ~GraphFilterGeometryStartLocation() = default;

        private:
            Node startNode;
            Node destinationNode;
            GraphFilterHierarchy *graphFilterHierarchy;
            double distanceKm;
            double secondDistanceKm;
            int firstMinFRC;
            int secondMinFRC;
            int levelOfDistance;

            bool ApplyFilter(Node &nodeFromGraph) {
                GPSDistanceCalculation::GPSDistanceCalculation distanceCalc;
                double distance = distanceCalc.DistanceEarth((double) startNode.GetLatitude(),
                                                             (double) startNode.GetLongitude(),
                                                             (double) nodeFromGraph.GetLatitude(),
                                                             (double) nodeFromGraph.GetLongitude());

                if (distance <= (this->distanceKm)) {
                    return true;
                }

                if (graphFilterHierarchy != nullptr) {
                    switch (levelOfDistance) {
                        case 1:
                            this->distanceKm = secondDistanceKm;
                            this->graphFilterHierarchy->SetMinFRC(firstMinFRC);
                            levelOfDistance++;
                            break;
                        case 2:
                            this->graphFilterHierarchy->SetMinFRC(secondMinFRC);
                            levelOfDistance++;
                            break;
                        case 3:
                            break;
                        default:
                            std::cout << "Unexpected state" << std::endl;
                            break;
                    }
                }

                return false;

            }

            void SetOwnData(const Node &destinationNode, Edge &edge) final {
                this->destinationNode = destinationNode;
            }

        };
    }
}

