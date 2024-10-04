#pragma once

#include <cmath>
#include "../../Data/Structures/Node.h"
#include "GraphFilterBase.h"
#include "../../Data/Utility/GPSDistanceCalculation.h"

using Routing::Node;

namespace Routing {

    namespace Algorithms {

        class GraphFilterGeometry : public GraphFilterBase {

        public:
            enum FilterType {
                NotSet, Circle, Ellipse, CircleStartNode
            };

            GraphFilterGeometry(FilterType typeFilter, Node &startNode, Node &endNode);

            double GetCircleMultiplier() {
                return this->radiusMultiplierCircle;
            };

            double GetEllipseMultiplier() {
                return this->radiusMultiplierEllipse;
            };

            void SetCircleMultiplier(double multiplier) {
                this->radiusMultiplierCircle = multiplier;
            }

            void SetEllipseMultiplier(double multiplier) {
                this->radiusMultiplierEllipse = multiplier;
            }

            void SetCenterCircle(double x, double y, double multipleRadius2){
                this->centerX = x;
                this->centerY = y;
                double r = sqrt(this->radius2);
                r = r * multipleRadius2;
                double newRadius2 = r * r;
                this->radius2 = newRadius2;
            }

            void DeleteAllFilter() final {
                if (nextFilter != nullptr) {
                    nextFilter->DeleteAllFilter();
                }

                delete nextFilter;
            }

            void Handle(bool &filtersPass) final {
                filtersPass = ApplyFilter(destinationNode);

                if (this->nextFilter != nullptr && filtersPass == true) {
                    nextFilter->Handle(filtersPass);
                }
            }

            ~GraphFilterGeometry() = default;

        private:
            FilterType typeFilter;
            Node startNode;
            Node endNode;
            Node destinationNode;

            //Circle
            double radiusMultiplierCircle;
            double centerX;
            double centerY;
            double radius2;

            //Ellipse
            std::tuple<float, float> ellipseFocusE;
            std::tuple<float, float> ellipseFocusF;
            double radiusMultiplierEllipse;
            double distanceEF;
            double ellipseK;

            bool ApplyFilter(Node &nodeFromGraph);

            bool IsInEllipse(Node &nodeFromGraph);

            bool IsInCircle(Node &nodeFromGraph);

            void SetOwnData(const Node &destinationNode, Edge &edge) final {
                this->destinationNode = destinationNode;
            }
        };
    }
}



