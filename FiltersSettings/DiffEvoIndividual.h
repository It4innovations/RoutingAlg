#pragma once
#include <iostream>

class DiffEvoIndividual {


public:
    double timeCost;
    int altCountCost;
    double firstDistanceKmLongPath;
    double secondDistanceKmLongPath;
    double firstDistanceKmShortPath;
    double secondDistanceKmShortPath;
    double fromXKmLongPath;
    int similarityPercent;
    bool someRouteNotFound;

    DiffEvoIndividual(){
        someRouteNotFound = false;
    }

    void SetParameter(int firstDistanceKmLongPath, int secondDistanceKmLongPath, int firstDistanceKmShortPath,
                      int secondDistanceKmShortPath, int fromXKmLongPath, int similarityPercent);

    void PrintParameters();

    void SetCost(double timeCost, int altCountCost){
        this->timeCost = timeCost;
        this->altCountCost = altCountCost;
    }

    double GetCost(){
        return timeCost;
    }
};

