#pragma once

#include <H5Cpp.h>

#include <memory>
#include <vector>
#include <ctime>
#include <iostream>

#include "ELoadType.h"
#include "DataIndexPart.h"
#include "DataIndex.h"
#include "DataIndexBase.h"

#include "NameConst.h"
#include "RoutingIndexInfo.h"
#include "../Data/Structures/NodePosition.h"

using Routing::Hdf::DataIndexPart;

namespace Routing {

    namespace Hdf {

        class DataIndex : public DataIndexBase {
        public:
            DataIndex(const H5::H5File &h5file, Routing::Hdf::ELoadType type = Routing::Hdf::ELoadType::All);

            time_t GetTimeCreated(void) const;

            DataIndexPart *GetIndexPart(int index) const;

            std::vector<NodePosition> GetNodeMap(void) const;

            std::vector<EdgeData> GetEdgeData(void) const;

            int GetIndexPartsCount(void) const;

            std::vector<PartInfo> GetPartInfo() const;

        private:
            time_t timeCreated;
            int indexPartsCount;
            int nodeMapCount;
            int edgeDataCount;

            std::vector<PartInfo> partInfo;

            std::vector<std::unique_ptr<DataIndexPart>> indexParts;

        };
    }
}

