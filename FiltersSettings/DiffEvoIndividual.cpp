#include "DiffEvoIndividual.h"

void DiffEvoIndividual::SetParameter(int firstDistanceKmLongPath, int secondDistanceKmLongPath, int firstDistanceKmShortPath,
                  int secondDistanceKmShortPath, int fromXKmLongPath, int similarityPercent){
    this->firstDistanceKmLongPath = firstDistanceKmLongPath;
    this->secondDistanceKmLongPath = secondDistanceKmLongPath;
    this->firstDistanceKmShortPath = firstDistanceKmShortPath;
    this->secondDistanceKmShortPath = secondDistanceKmShortPath;
    this->fromXKmLongPath = fromXKmLongPath;
    this->similarityPercent = similarityPercent;
}

void DiffEvoIndividual::PrintParameters(){
    std::cout << "Individual Parameters:" << std::endl;
    std::cout << "--+--+--+--firstDistanceKmLongPath: " << firstDistanceKmLongPath << std::endl;
    std::cout << "  |  |  +--secondDistanceKmLongPath: " << secondDistanceKmLongPath << std::endl;
    std::cout << "  |  +--fromXKmLongPath: " << fromXKmLongPath << std::endl;
    std::cout << "  |  +--+--firstDistanceKmShortPath: " << firstDistanceKmShortPath << std::endl;
    std::cout << "  |     +--secondDistanceKmShortPath: " << secondDistanceKmShortPath << std::endl;
    std::cout << "  +--+--timeCost: " << timeCost << std::endl;
    std::cout << "     +--altCountCost: " << altCountCost << std::endl << std::endl;
}