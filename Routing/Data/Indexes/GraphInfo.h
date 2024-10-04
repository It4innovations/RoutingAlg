#pragma once

#include <memory>
#include <vector>
#include <ctime>

namespace Routing {

    namespace Data {
        // TODO refactoring
        class GraphInfo {
        public:
            GraphInfo() {
                this->timeCreated = 0;
                this->partCount = 0;
                this->edgeCount = 0;
                this->edgeDataCount = 0;
                this->nodeCount = 0;
            };

            GraphInfo(const time_t &timeCreated, int partCount) {
                this->timeCreated = timeCreated;
                this->partCount = partCount;
                this->edgeCount = 0;
                this->edgeDataCount = 0;
                this->nodeCount = 0;
            };

            time_t GetTimeCreated(void) const { return this->timeCreated; };

            int GetNodeCount(void) const { return this->nodeCount; };

            int GetEdgeCount(void) const { return this->edgeCount; };

            int GetEdgeDataCount(void) const { return this->edgeDataCount; };

            int GetPartCount(void) const { return this->partCount; };

        private:
            int nodeCount;
            int edgeCount;
            int edgeDataCount;
            int partCount;

            time_t timeCreated;

        };
    }
}


