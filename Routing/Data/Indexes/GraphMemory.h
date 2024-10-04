#pragma once

#include <atomic>
#include <memory>
#include <string>
#include <ostream>
#include <iterator>
#include <cassert>
#include <map>

#include "GraphPartMemory.h"
#include "GraphInfo.h"
#include "../Structures/Edge.h"
#include "../Utility/TimeWatch.h"
#include "../../Algorithms/Common/Segment.h"
#include "../../Data/Exceptions/Exceptions.h"
#include "../../Hdf/DataIndexPart.h"
#include "../../Hdf/DataIndex.h"
#include <sparsehash/dense_hash_map>
#include "../Exceptions/Exceptions.h"

using Routing::Utils::TimeWatch;
using Routing::Algorithms::Segment;
using Routing::Hdf::DataIndexPart;
using Routing::Hdf::DataIndex;
using Routing::Exception::NodeNotFoundException;
using Routing::Exception::EdgeNotFoundException;

namespace Routing {

    namespace Data {
        class GraphMemory {
        public:
            ~GraphMemory();

            GraphMemory(DataIndex &indexData);

            bool ContainsNode(int nodeId);

            const Node &GetNodeById(int nodeId);

            const Node &GetEndNodeByEdge(const Edge &edge);

            const Node &GetStartNodeByEdge(const Edge &edge);

            const Edge &GetEdgeById(int edgeId);

            const std::vector<Edge *> GetEdgesIn(int nodeId);

            const std::vector<Edge *> GetEdgesOut(int nodeId);

            const std::vector<Edge *> GetEdgesOut(const Node &node);

            Segment EdgeToSegment(int edgeId);

            Segment EdgeToSegment(const Edge &edge);

            int GetNodeCount();

            int GetEdgeCount();

            void SetEdgeSpeed(int edgeId, float speed);

            const GpsRectangle &GetBoundingBox();

            std::vector<GpsRectangle> GetBoundingBoxes();

            void ResetGraphSpeed();

            std::ostream &operator<<(std::ostream &os) {
                os << "P: " << this->parts.size() << ", N: " << this->GetNodeCount() << ", E: "
                   << this->GetEdgeCount() << ", IdMap: " << this->nodeIdStore.size() <<
                   std::endl;

                return os;
            };

            google::dense_hash_map<int, const Node *> nodeIdStore;
            google::dense_hash_map<int, const Edge *> edgeIdStore;

            const std::vector<EdgeData> edgeData;
        private:
            GraphInfo info;
            std::map<std::string, std::shared_ptr<GraphPartMemory>> parts;
            GpsRectangle boundingBox;
        };

    }
}


