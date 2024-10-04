
#include "TDUtility.h"

float Routing::Algorithms::TD::CalculateCostSecPrecise(const Edge &edge, float startTime,
                                                       std::shared_ptr<std::vector<float>> edgeSpeedProfile,
                                                       std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5> storage) {

    int edgeLength = edge.length;

    if (((int) startTime >= (SECONDSINWEEK))) {
        startTime = (int) startTime % SECONDSINWEEK;
    }

    unsigned long index = (unsigned long) startTime / storage->GetIntervalLength();
    float speedMetersPerSecond = edgeSpeedProfile->at(index);
    float timeSpend = edgeLength / speedMetersPerSecond;
    float finishTime = startTime + timeSpend;

    unsigned long newIndex = (unsigned long) finishTime / storage->GetIntervalLength();

    if (index == newIndex) {
        return timeSpend;
    }

    index++;

    float result = 0;
    int restLength = edgeLength;
    float indexPartTime = index * storage->GetIntervalLength() - startTime;
    result += indexPartTime;
    float distancePartTravel = speedMetersPerSecond * indexPartTime;
    restLength -= (int) distancePartTravel;

    if (index == storage->GetIntervalCount()) {
        index = 0;
    }

    while (true) {
        speedMetersPerSecond = edgeSpeedProfile->at(index);
        timeSpend = restLength / speedMetersPerSecond;

        if (timeSpend < storage->GetIntervalLength()) {
            return result + timeSpend;
        }

        result += storage->GetIntervalLength();
        restLength -= storage->GetIntervalLength() * speedMetersPerSecond;
        index++;

        if (index == storage->GetIntervalCount()) {
            index = 0;
        }
    }
}


float Routing::Algorithms::TD::CalculateCostSec(const Edge &edge, float speedKmPH) {
    auto res = edge.length / (speedKmPH / (float) 3.6);
    return res;
}

void
Routing::Algorithms::TD::CalculateRoadCostByProfile(std::vector<Segment> &route, int startTime,
                                                    const std::shared_ptr<Routing::Data::GraphMemory>& routingGraph,
                                                    const std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5>& storage) {

    float previousTime = startTime;

    for (auto &segment : route) {
        float travelTime;

        try {
            auto profile = storage->GetSpeedProfileByEdgeId(segment.edgeId);
            travelTime = Routing::Algorithms::TD::CalculateCostSecPrecise(routingGraph->GetEdgeById(segment.edgeId),
                                                                          previousTime, profile, storage);
        }
        catch (Routing::Exception::ProfileNotFound &e) {
            travelTime = Routing::Algorithms::TD::CalculateCostSec(routingGraph->GetEdgeById(segment.edgeId),
                                                                   routingGraph->GetEdgeById(segment.edgeId).GetSpeed());
        }

        previousTime = previousTime + travelTime;
        segment.time = previousTime;
    }
}

std::vector<int> Routing::Algorithms::TD::GetNumbersFromFile(const std::string& filePath) {

    std::vector<int> result;

    std::ifstream file(filePath);

    int number;

    while (!file.eof()) {
        file >> number;
        result.push_back(number);
    }

    return result;
}


