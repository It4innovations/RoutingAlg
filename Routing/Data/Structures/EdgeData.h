#pragma once

#include <limits>
#include <ostream>

#include "../Enums/EVehiclesAccess.h"
#include "../Enums/ESpecificInfo.h"

namespace Routing {

    class EdgeData {

    public:
        float speed;
        unsigned char funcClass;
        unsigned char lanes;
        signed char incline;
        EVehiclesAccess vehicleAccess;
        ESpecificInfo specificInfo;

        uint16_t maxWeight = std::numeric_limits<uint16_t>::max();
        uint16_t maxHeight = std::numeric_limits<uint16_t>::max();
        unsigned char maxAxleLoad = std::numeric_limits<unsigned char>::max();
        unsigned char maxWidth = std::numeric_limits<unsigned char>::max();
        unsigned char maxLength = std::numeric_limits<unsigned char>::max();

        EdgeData() {};

        EdgeData(float speed, unsigned char funcClass, unsigned char lanes, EVehiclesAccess vehAccess,
                 ESpecificInfo specInfo) :
                speed(speed),
                funcClass(funcClass),
                lanes(lanes),
                incline(0),
                vehicleAccess(vehAccess),
                specificInfo(specInfo) {};

        EdgeData(float speed, unsigned char funcClass, unsigned char lanes, EVehiclesAccess vehAccess,
                 ESpecificInfo specInfo,
                 unsigned short maxWeight, unsigned short maxHeight, unsigned char maxAxleLoad, unsigned char maxWidth,
                 unsigned char maxLength, signed char incline) :

                EdgeData(speed, funcClass, lanes, vehAccess, specInfo) {
            this->maxWeight = maxWeight;
            this->maxHeight = maxHeight;
            this->maxAxleLoad = maxAxleLoad;
            this->maxWidth = maxWidth;
            this->maxLength = maxLength;
            this->incline = incline;
        };

        EdgeData(float speed, unsigned char funcClass, unsigned char lanes, EVehiclesAccess vehAccess,
                 ESpecificInfo specInfo,
                 float maxWeight, float maxHeight, float maxAxleLoad, float maxWidth, float maxLength,
                 signed char incline) :
                EdgeData(speed, funcClass, lanes, vehAccess, specInfo) {
            this->SetMaxWeight(maxWeight);
            this->SetMaxHeight(maxHeight);
            this->SetMaxAxleLoad(maxAxleLoad);
            this->SetMaxWidth(maxWidth);
            this->SetMaxLenght(maxLength);
            this->incline = incline;
        };


        EdgeData(const EdgeData &other) :
                speed(other.speed),
                funcClass(other.funcClass),
                lanes(other.lanes),
                incline(other.incline),
                vehicleAccess(other.vehicleAccess),
                specificInfo(other.specificInfo),
                maxWeight(other.maxWeight),
                maxHeight(other.maxHeight),
                maxAxleLoad(other.maxAxleLoad),
                maxWidth(other.maxWidth),
                maxLength(other.maxLength) {};

        EdgeData(EdgeData &&other) {
            this->speed = other.speed;
            this->funcClass = other.funcClass;
            this->lanes = other.lanes;
            this->incline = other.incline;
            this->vehicleAccess = other.vehicleAccess;
            this->specificInfo = other.specificInfo;
            this->maxWeight = other.maxWeight;
            this->maxHeight = other.maxHeight;
            this->maxAxleLoad = other.maxAxleLoad;
            this->maxWidth = other.maxWidth;
            this->maxLength = other.maxLength;
        };

        friend std::ostream &operator<<(std::ostream &os, const EdgeData &edgeData) {
            os << "S: " << edgeData.speed << ", F: " << int(edgeData.funcClass) << ", L: "
               << int(edgeData.lanes) << ", A: " << int(edgeData.vehicleAccess) << ", I: "
               << int(edgeData.specificInfo);

            return os;
        }

        inline float GetMaxWeight(void) const { return this->maxWeight / DIM_CONS_GET; };

        inline unsigned short GetMaxWeightRaw(void) const { return this->maxWeight; };

        void SetMaxWeight(float value) { this->maxWeight = static_cast<unsigned short>(value / DIM_CONS_SET); };

        inline float GetMaxHeight(void) const { return this->maxHeight / DIM_CONS_GET; };

        inline unsigned short GetMaxHeightRaw(void) const { return this->maxHeight; };

        void SetMaxHeight(float value) { this->maxHeight = static_cast<unsigned short>(value / DIM_CONS_SET); };

        inline float GetMaxAxleLoad(void) const { return this->maxAxleLoad / DIM_CONS_GET; };

        inline unsigned char GetMaxAxleLoadRaw(void) const { return this->maxAxleLoad; };

        void SetMaxAxleLoad(float value) { this->maxAxleLoad = static_cast<unsigned char>(value / DIM_CONS_SET); };

        inline float GetMaxWidth(void) const { return this->maxWidth / DIM_CONS_GET; };

        inline unsigned char GetMaxWidthRaw(void) const { return this->maxWidth; };

        void SetMaxWidth(float value) { this->maxWidth = static_cast<unsigned char>(value / DIM_CONS_SET); };

        inline float GetMaxLenght(void) const { return this->maxLength / DIM_CONS_GET; };

        inline unsigned char GetMaxLenghtRaw(void) const { return this->maxLength; };

        void SetMaxLenght(float value) { this->maxLength = static_cast<unsigned char>(value / DIM_CONS_SET); };

    private:


        const float DIM_CONS_GET = 10.0;
        const int DIM_CONS_SET = 10;
    };
}


