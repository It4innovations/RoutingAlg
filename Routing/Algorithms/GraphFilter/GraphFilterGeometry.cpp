
#include "GraphFilterGeometry.h"

using Routing::Algorithms::GraphFilterGeometry;

GraphFilterGeometry::GraphFilterGeometry(FilterType typeFilter, Node &startNode, Node &endNode) {
    this->typeFilter = typeFilter;
    this->startNode = startNode;
    this->endNode = endNode;

    if(typeFilter == Circle){
        this->radiusMultiplierCircle = 1.8;
        this->centerX = (startNode.GetLongitude() + endNode.GetLongitude()) / 2;
        this->centerY = (startNode.GetLatitude() + endNode.GetLatitude()) / 2;
        double r = sqrt(pow(abs(startNode.GetLongitude() - centerX), 2) +
                        pow(abs(startNode.GetLatitude() - centerY), 2)) * radiusMultiplierCircle;
        this->radius2 = r * r;
    }

    if (typeFilter == Ellipse) {
        GPSDistanceCalculation::GPSDistanceCalculation distanceCalc;
        this->radiusMultiplierEllipse = 1.25;
        this->ellipseFocusE = std::make_tuple(startNode.GetLongitude(),
                                              startNode.GetLatitude()); //[x1,y1] of focus E
        this->ellipseFocusF = std::make_tuple(endNode.GetLongitude(),
                                              endNode.GetLatitude()); //[x2,y2] of focus F
        this->distanceEF = distanceCalc.DistanceEarth((double) std::get<1>(this->ellipseFocusE),
                                                      (double) std::get<0>(this->ellipseFocusE),
                                                      (double) std::get<1>(this->ellipseFocusF),
                                                      (double) std::get<0>(this->ellipseFocusF));


        this->ellipseK = distanceEF * (this->radiusMultiplierEllipse); //ellipse constant K

        double threeKmDistance = 3;
        double fifteenKmDistance = 15;
        double fiftyKmDistance = 50;

        if (threeKmDistance > distanceEF) {
            this->ellipseK = threeKmDistance * 2;
        } else {
            if (fifteenKmDistance > distanceEF) {
                this->radiusMultiplierEllipse = 2.5;
                this->ellipseK = distanceEF * radiusMultiplierEllipse;
            } else {
                if (fiftyKmDistance > distanceEF) {
                    this->radiusMultiplierEllipse = 1.75;
                    this->ellipseK = distanceEF * radiusMultiplierEllipse;
                }
            }
        }
    }
}

bool GraphFilterGeometry::ApplyFilter(Node &nodeFromGraph) {
    if (this->typeFilter == Ellipse) {
        return IsInEllipse(nodeFromGraph);
    }

    if (this->typeFilter == Circle) {
        return IsInCircle(nodeFromGraph);
    }

    return true; //none filter
}

bool GraphFilterGeometry::IsInEllipse(Node &nodeFromGraph) {
    GPSDistanceCalculation::GPSDistanceCalculation distanceCalc;

    double pointX = nodeFromGraph.GetLongitude();
    double pointY = nodeFromGraph.GetLatitude();

    double distanceXE = distanceCalc.DistanceEarth((double) std::get<1>(this->ellipseFocusE),
                                                   (double) std::get<0>(this->ellipseFocusE), pointY, pointX);
    double distanceXF = distanceCalc.DistanceEarth((double) std::get<1>(this->ellipseFocusF),
                                                   (double) std::get<0>(this->ellipseFocusF), pointY, pointX);

    return (distanceXE + distanceXF <= (this->ellipseK));
}

bool GraphFilterGeometry::IsInCircle(Node &nodeFromGraph) {

    double pointX = nodeFromGraph.GetLongitude();
    double pointY = nodeFromGraph.GetLatitude();
    double dx = pointX - this->centerX;
    double dy = pointY - this->centerY;

    return pow(dx, 2) + pow(dy, 2) <= (this->radius2);
}
