#pragma once

#include <atomic>
#include <memory>
#include <vector>

#include "../Enums/ESpecificInfo.h"
#include "../Enums/EVehiclesAccess.h"
#include "../Structures/EdgeData.h"
#include "../Structures/Node.h"

#define RESTART_SPEED 100000

using namespace std;

namespace Routing {
    struct Node;

    struct Edge final {
    public:
        int edgeId;
        int length;

        const EdgeData* edgeData = nullptr;

        union {
            int endNodeIndex;
            Node *endNodePtr;
        } endNode;

        Node *startNodePtr;

        Edge(int edgeId, int computed_speed, int length, const EdgeData *edgeData, int endNodeIndex) :
                edgeId(edgeId),
                length(length),
                edgeData(edgeData),
                startNodePtr(nullptr),
                speed(computed_speed),
                origin_speed(computed_speed) {
            this->endNode.endNodeIndex = endNodeIndex;
        };

        Edge() = default;
        Edge(Edge &&other) = delete;

        Edge(const Edge &other) :
                edgeData(other.edgeData),
                endNode(other.endNode) {
            this->edgeId = other.edgeId;
            this->length = other.length;
            this->speed = other.speed;
            this->origin_speed = other.origin_speed;
            //TODO both nodes ptr, edgeData
        }
        Edge &operator=(const Edge &other) {
            this->edgeId = other.edgeId;
            this->length = other.length;
            this->speed = other.speed;
            this->origin_speed = other.origin_speed;
            //TODO
            //this->edgeData = other.edgeData;
            //this->endNode = other.endNode
            return *this;
        }

        bool operator==(const Edge &other) const {
            return this->edgeId == other.edgeId && this->length == other.length &&
                   this->speed == other.speed &&
                   this->origin_speed == other.origin_speed &&
                   this->edgeData == other.edgeData && this->endNode.endNodePtr == other.endNode.endNodePtr;
        }

        friend std::ostream &operator<<(std::ostream &os, const Edge &edge) {
            os << "EId: " << edge.edgeId
               << // " En: " << edge.endNode->endNodePtr->id << " Sn: " << edge.startNodePtr->id <<
               ", Length: " << edge.length << ", Edata: " << edge.edgeData <<
               ", speed: " << edge.speed << std::endl;

            return os;
        }

        float GetSpeed() const {
            return this->speed;
        };

        void SetSpeed(int new_speed) {
            if (new_speed == RESTART_SPEED) {
                this->ResetSpeed();
            }
            else this->speed = new_speed;
        }

        void ResetSpeed() {
            this->speed = this->origin_speed;
        };

        float GetSpeedMPS() const { return this->edgeData->speed / SPEED_CONS; };

        unsigned char GetFuncClass() const { return this->edgeData->funcClass; };

        unsigned char GetLanes() const { return this->edgeData->lanes; };

        ESpecificInfo GetSpecificInfo() const { return this->edgeData->specificInfo; };

        EVehiclesAccess GetVehicleAccess() const { return this->edgeData->vehicleAccess; };

        float GetMaxWeight() const { return this->edgeData->GetMaxWeight(); };

        float GetMaxHeight() const { return this->edgeData->GetMaxHeight(); };

        float GetMaxAxleLoad() const { return this->edgeData->GetMaxAxleLoad(); };

        float GetMaxWidth() const { return this->edgeData->GetMaxWidth(); };

        float GetMaxLength() const { return this->edgeData->GetMaxLenght(); };

        signed char GetIncline() const { return this->edgeData->incline; };

        const EdgeData& GetEdgeData() const { return *this->edgeData; };

    private:
        int speed;
        int origin_speed;
        constexpr static float SPEED_CONS = 3.6;
    };

    struct EdgeHash {
        std::size_t operator()(const Routing::Edge *e) const {
            return std::hash<int>()(e->edgeId) /* ^ std::hash<int>()(e->endNode->id) */ ^ std::hash<int>()(e->length);
        }

        std::size_t operator()(const Routing::Edge &e) const {
            return std::hash<int>()(e.edgeId) /* ^ std::hash<int>()(e.endNode->id) */ ^ std::hash<int>()(e.length);
        }
    };
}
