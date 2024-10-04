#include <utility>

#pragma once

#include <memory>
#include <vector>

#include "../Common/AlgorithmSettings.h"
#include "../../Data/Exceptions/Exceptions.h"
#include "../../Data/Indexes/GraphMemory.h"


using Routing::Data::GraphMemory;
using Routing::Exception::NodeNotFoundException;

namespace Routing {

    namespace Algorithms {

        class Algorithm {
        public:
            explicit Algorithm(std::shared_ptr<GraphMemory> routingGraph,
                    AlgorithmSettings settings = AlgorithmSettings()) : routingGraph(std::move(routingGraph)),
                                                                          settings(settings) {}

            std::shared_ptr<Routing::Data::GraphMemory> GetRoutingGraph() {
                return routingGraph;
            }

            AlgorithmSettings GetAlgorithmSettings() {
                return settings;
            }

            void SetAlgorithmSettings(const AlgorithmSettings& settings) {
                this->settings = settings;
            }

        protected:

            std::shared_ptr<Routing::Data::GraphMemory> routingGraph;

            AlgorithmSettings settings;

            bool IsTollWayPassable(const ESpecificInfo &info) const {
                if (!this->settings.useTollWays) {
                    return !IsTollWay(info);
                }

                return true;
            }

            bool IsTollWay(const ESpecificInfo &info) const {
                return (info & ESpecificInfo::TollWay) == ESpecificInfo::TollWay;
            }

            //#REFACTORING# -- useless
            bool IsVehicleAccessible(const EVehiclesAccess access) const {
                //TODO ACCESS
                return true;
                //return (access & this->settings.vehicleAccess) == this->settings.vehicleAccess;
            }

            bool IsVehiclePassable(const EdgeData &data) const {
                return data.GetMaxAxleLoad() >= this->settings.vehicleAxleLoad &&
                       data.GetMaxHeight() >= this->settings.vehicleHeight &&
                       data.GetMaxLenght() >= this->settings.vehicleLength &&
                       data.GetMaxWeight() >= this->settings.vehicleWeight &&
                       data.GetMaxWidth() >= this->settings.vehicleWidth;
            }

            bool IsAccessible(const EdgeData &data) const {
                return this->IsTollWayPassable(data.specificInfo) &&
                       this->IsVehicleAccessible(data.vehicleAccess) &&
                       this->IsVehiclePassable(data);
            }
        };
    }
}
