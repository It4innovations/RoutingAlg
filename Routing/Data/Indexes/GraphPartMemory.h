#pragma once

#include <memory>
#include <vector>
#include <unordered_set>
#include <algorithm>

#include "../Structures/NodeRaw.h"
#include "../Structures/Node.h"
#include "../Structures/Geo/GpsRectangle.h"
#include "../Structures/EdgeRaw.h"

namespace Routing {

    namespace Data {

        class GraphPartMemory {
        public:
            GraphPartMemory(const std::vector<Node> &nodes, const std::vector<Edge> &edges,
                            const GpsRectangle &boundingBox);

            GraphPartMemory(const std::vector<NodeRaw> &nodesRaw,
                            const std::vector<EdgeRaw> &edgesRaw,
                            const std::vector<EdgeData> &edgeData,
                            Routing::GpsRectangle &&boundingBox);

            unsigned long GetNodeCount(void);

            unsigned long GetEdgeCount(void);

            const GpsRectangle &GetBoundingBox(void) const;

            const Node *GetNodePtr(int nodePos) const;

            const Edge *GetEdgePtr(int edgePtr) const;

            void ResetSpeed();

            void PrintNodes();

            void PrintEdges();

        private:
            std::vector<Node> nodes;
            std::vector<Edge> edges;
            GpsRectangle boundingBox;
        };
    }
}


