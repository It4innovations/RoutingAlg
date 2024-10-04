
#include "DataIndex.h"
#include "Types/RoutingIndexInfoCompType.h"
#include <iomanip>
#include <cstring>

Routing::Hdf::DataIndex::DataIndex(const H5::H5File &h5file, Routing::Hdf::ELoadType type) : DataIndexBase(h5file) {
    // Read index info attribute
    try {
        H5::Attribute timeCreate = h5file.openGroup(Routing::Hdf::indexGroupPath).openAttribute(
                Routing::Hdf::indexCreateAttName);

        timeCreate.read(IndexH5Type::RoutingIndexCreateType(), &this->timeCreated);
        timeCreate.close();

        H5::Attribute partCount = h5file.openGroup(Routing::Hdf::indexGroupPath).openAttribute(
                Routing::Hdf::indexPartCountAttName);
        partCount.read(IndexH5Type::RoutingIndexCountType(), &this->indexPartsCount);
        partCount.close();

        H5::Attribute partInfo = h5file.openGroup(Routing::Hdf::indexGroupPath).openAttribute(
                Routing::Hdf::indexPartsAttName);

        for (int i = 0; i < partInfo.getSpace().getSimpleExtentNpoints(); ++i) {
            PartInfo pi;
            partInfo.read(IndexH5Type::RoutingIndexPartsCompType(), &pi);

#ifndef NDEBUG
            std::cout << "Got part " << pi.idName << std::endl;
#endif

            this->partInfo.push_back(pi);
        }

        // Node map count
        H5::DataSpace dsNode = h5file.openGroup(Routing::Hdf::indexGroupPath).openDataSet(
                Routing::Hdf::nodeMapDataSetName).getSpace();
        hsize_t *dimSizesTmpNm = new hsize_t[dsNode.getSimpleExtentNdims()];

        dsNode.getSimpleExtentDims(dimSizesTmpNm);
        this->nodeMapCount = dimSizesTmpNm[0];

        delete[] dimSizesTmpNm;

        //Edge data
        H5::DataSpace dsEmap = h5file.openGroup(Routing::Hdf::indexGroupPath).openDataSet(
                Routing::Hdf::edgeDataDataSetName).getSpace();
        hsize_t *dimSizesTmpEd = new hsize_t[dsEmap.getSimpleExtentNdims()];

        dsEmap.getSimpleExtentDims(dimSizesTmpEd);
        this->edgeDataCount = dimSizesTmpEd[0];

        delete[] dimSizesTmpEd;

    }
    catch (const H5::Exception e) {
        std::cerr << "DataIndex constructor\n" << e.getDetailMsg();
        exit(EXIT_FAILURE);
    }

    if ((type & Routing::Hdf::ELoadType::Graph) == Routing::Hdf::ELoadType::Graph) {
        this->indexParts.reserve(this->indexPartsCount);

        for (auto &&part : partInfo) {
            this->indexParts.emplace_back(std::unique_ptr<DataIndexPart>(new DataIndexPart(h5file, part.idName)));
        }
    }
}

time_t Routing::Hdf::DataIndex::GetTimeCreated(void) const {
    return this->timeCreated;
}

std::vector<Routing::NodePosition> Routing::Hdf::DataIndex::GetNodeMap(void) const {
    std::vector<Routing::NodePosition> nodeMap(this->nodeMapCount);

    try {
        H5::DataSet nodesDS = h5file.openGroup(Routing::Hdf::indexGroupPath).openDataSet(
                Routing::Hdf::nodeMapDataSetName);

        nodesDS.read(nodeMap.data(), IndexH5Type::NodeMapType());
    }
    catch (H5::FileIException &e) {
        std::cerr << "ERROR: Cannot open file. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::DataSetIException &e) {
        std::cerr << "ERROR: Cannot open dataset. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::GroupIException &e) {
        std::cerr << "ERROR: Cannot open group. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::DataSpaceIException &e) {
        std::cerr << "ERROR: Cannot open dataspace. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }

    auto getInd = [this](Routing::NodePosition &node) -> void {
        for (auto &&pt : this->partInfo) {
            if (!std::strcmp(pt.idName, node.partId)) {
                std::strncpy(node.partId, pt.idName, ID_INFO_NAME_SZ);
                return;
            }
        }

        std::cerr << "ERROR: part id " << node.partId << " for node " << node.nodeId << " not found.";
        exit(EXIT_FAILURE);
    };

    std::for_each(nodeMap.begin(), nodeMap.end(), getInd);

    return nodeMap;
}

std::vector<EdgeData> Routing::Hdf::DataIndex::GetEdgeData(void) const {
    std::vector<EdgeData> edgeData(this->edgeDataCount);

    try {
        H5::DataSet edgeDataDS = h5file.openGroup(Routing::Hdf::indexGroupPath).openDataSet(
                Routing::Hdf::edgeDataDataSetName);
        edgeDataDS.read(edgeData.data(), IndexH5Type::EdgeDataType());
    }
    catch (H5::FileIException &e) {
        std::cerr << "ERROR: Cannot open file. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::DataSetIException &e) {
        std::cerr << "ERROR: Cannot open dataset. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::GroupIException &e) {
        std::cerr << "ERROR: Cannot open group. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::DataSpaceIException &e) {
        std::cerr << "ERROR: Cannot open dataspace. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }

    return edgeData;
}

int Routing::Hdf::DataIndex::GetIndexPartsCount(void) const {
    return this->indexPartsCount;
}

DataIndexPart *Routing::Hdf::DataIndex::GetIndexPart(int index) const {
    return this->indexParts[index].get();
}


std::vector<Routing::PartInfo> Routing::Hdf::DataIndex::GetPartInfo() const {
    return this->partInfo;
}
