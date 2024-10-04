#pragma once

#include <memory>
#include <vector>
#include <H5Cpp.h>

#include "NameConst.h"
#include "DataIndexBase.h"
#include "../Data/Structures/NodeRaw.h"
#include "../Data/Structures/EdgeRaw.h"
#include "../Data/Structures/EdgeData.h"
#include "RoutingIndexInfo.h"

using Routing::NodeRaw;
using Routing::EdgeRaw;
using Routing::EdgeData;

namespace Routing {

    namespace Hdf {

        class DataIndexPart : public DataIndexBase {
        public:
            DataIndexPart(const H5::H5File &h5file, char id_name[ID_INFO_NAME_SZ]);

            int GetPartNumber(void) const;

            float GetLeft(void) const;

            float GetTop(void) const;

            float GetRight(void) const;

            float GetBottom(void) const;

            std::vector<NodeRaw> GetNodes(void) const;

            std::vector<EdgeRaw> GetEdges(void) const;

            std::vector<EdgeRaw> GetEdges(int index, int count) const;

            int GetNodeCount(void) const;

            int GetEdgeCount(void) const;

            NodeRaw GetNode(int index) const;

            EdgeRaw GetEdge(int index) const;

        private:
            std::string id_name;
            float left;
            float top;
            float right;
            float bottom;

            int edgeCount;
            int nodeCount;
        };
    }
}


