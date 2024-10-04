#pragma once

#include "../../Data/Utility/Constants.h"
#include "../../Data/Structures/Edge.h"


namespace Routing {

    enum class CostCalcType {
        Time_cost = 0,
        Len_cost = 1
    };

    class CostCalculator {
    public:
        CostCalculator(CostCalcType type, unsigned char maxVelocity) :
                type(type),
                maxVelocity(maxVelocity) {};

        std::pair<float, float> CalculateCost(const Routing::Edge &edge) const {
            float travelTime = this->CalculateCostTime(edge);

            switch (this->type) {
                case CostCalcType::Time_cost:
                    return {travelTime, travelTime};

                case CostCalcType::Len_cost:
                    return {edge.length, travelTime};

                default:
                    //TODO exception
                    return {std::numeric_limits<float>::min(), std::numeric_limits<float>::min()};
            }
        }

    private:
        float CalculateCostTime(const Edge &edge) const {
            float speed = this->maxVelocity < edge.GetSpeed() ? this->maxVelocity :
                          edge.GetSpeed() * Routing::Constants::OneDiv3P6;

            return edge.length / speed;
        }

        CostCalcType type;
        unsigned char maxVelocity;
    };
}


