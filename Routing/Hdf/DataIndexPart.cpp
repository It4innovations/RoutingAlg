
#include "Types/RoutingIndexInfoCompType.h"
#include "DataIndexPart.h"

Routing::Hdf::DataIndexPart::DataIndexPart(const H5::H5File &h5file, char id_name[ID_INFO_NAME_SZ]) :
        DataIndexBase(h5file),
        id_name(id_name) {
    // Read index info attribute
    try {
        // Edge count
        H5::DataSpace ds = h5file.openGroup(Routing::Hdf::indexGroupPath + this->id_name).openDataSet(
                Routing::Hdf::edgeDataSetName).getSpace();
        int rank = ds.getSimpleExtentNdims();
        hsize_t *dimSizesTmp = new hsize_t[rank];
        ds.getSimpleExtentDims(dimSizesTmp);
        this->edgeCount = dimSizesTmp[0];
        delete[] dimSizesTmp;

        // Node count
        ds = h5file.openGroup(Routing::Hdf::indexGroupPath + this->id_name).openDataSet(
                Routing::Hdf::nodeDataSetName).getSpace();
        dimSizesTmp = new hsize_t[ds.getSimpleExtentNdims()];
        ds.getSimpleExtentDims(dimSizesTmp);
        this->nodeCount = dimSizesTmp[0];
        delete[] dimSizesTmp;
    }
    catch (const H5::Exception e) {
        std::cerr << "DataIndexPart constructor\n" << e.getDetailMsg();
    }

}

float Routing::Hdf::DataIndexPart::GetLeft(void) const {
    return this->left;
}

float Routing::Hdf::DataIndexPart::GetTop(void) const {
    return this->top;
}

float Routing::Hdf::DataIndexPart::GetRight(void) const {
    return this->right;
}

float Routing::Hdf::DataIndexPart::GetBottom(void) const {
    return this->bottom;
}

std::vector<NodeRaw> Routing::Hdf::DataIndexPart::GetNodes(void) const {
    std::vector<NodeRaw> nodes(nodeCount);

    try {
        H5::DataSet nodesDS = h5file.openGroup(Routing::Hdf::indexGroupPath + this->id_name).openDataSet(
                Routing::Hdf::nodeDataSetName);
        nodesDS.read(nodes.data(), IndexH5Type::NodeRawType());
    }
    catch (H5::FileIException e) {
        std::cerr << "ERROR: Cannot open file. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::DataSetIException e) {
        std::cerr << "ERROR: Cannot open dataset. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::GroupIException e) {
        std::cerr << "ERROR: Cannot open group. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::DataSpaceIException e) {
        std::cerr << "ERROR: Cannot open dataspace. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }

    return nodes;

}

std::vector<EdgeRaw> Routing::Hdf::DataIndexPart::GetEdges(void) const {
    std::vector<EdgeRaw> edgesRaw(edgeCount);

    try {
        H5::DataSet edgesDS = h5file.openGroup(Routing::Hdf::indexGroupPath + this->id_name).openDataSet(
                Routing::Hdf::edgeDataSetName);
        edgesDS.read(edgesRaw.data(), IndexH5Type::EdgeRawType());
    }
    catch (H5::FileIException e) {
        std::cerr << "ERROR: Cannot open file. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::DataSetIException e) {
        std::cerr << "ERROR: Cannot open dataset. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::GroupIException e) {
        std::cerr << "ERROR: Cannot open group. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::DataSpaceIException e) {
        std::cerr << "ERROR: Cannot open dataspace. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }

    return edgesRaw;
}

std::vector<EdgeRaw> Routing::Hdf::DataIndexPart::GetEdges(int index, int count) const {
    std::vector<EdgeRaw> edges(count);
    if (index + count > edgeCount - 1)
        std::cerr << "ERROR: Selected part (index " << index << ", count " << count << ") ends outside of the dataset."
                  << std::endl;

    try {
        // Open dataset
        H5::DataSet edgesDS = h5file.openGroup(Routing::Hdf::indexGroupPath + this->id_name).openDataSet(
                Routing::Hdf::edgeDataSetName);
        H5::DataSpace edgesDSspace = edgesDS.getSpace();
        hsize_t *start = new hsize_t[edgesDSspace.getSimpleExtentNdims()]{static_cast<hsize_t>(index), 1};
        hsize_t *offset = new hsize_t[edgesDSspace.getSimpleExtentNdims()]{static_cast<hsize_t>(count), 1};
        edgesDSspace.selectHyperslab(H5S_SELECT_SET, start, offset);
        edgesDS.read(edges.data(), IndexH5Type::EdgeRawType(), edgesDSspace);
        delete[] start;
        delete[] offset;
    }
    catch (H5::FileIException e) {
        std::cerr << "ERROR: Cannot open file. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::DataSetIException e) {
        std::cerr << "ERROR: Cannot open dataset. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::GroupIException e) {
        std::cerr << "ERROR: Cannot open group. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::DataSpaceIException e) {
        std::cerr << "ERROR: Cannot open dataspace. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }

    return edges;
}

int Routing::Hdf::DataIndexPart::GetNodeCount(void) const {
    return this->nodeCount;
}

int Routing::Hdf::DataIndexPart::GetEdgeCount(void) const {
    return this->edgeCount;
}

NodeRaw Routing::Hdf::DataIndexPart::GetNode(int index) const {
    NodeRaw node;
    if (index > nodeCount - 1)
        std::cerr << "ERROR: Selected node (index " << index << ") is outside of the dataset." << std::endl;

    try {
        // Open dataset
        H5::DataSet nodesDS = h5file.openGroup(Routing::Hdf::indexGroupPath + this->id_name).openDataSet(
                Routing::Hdf::nodeDataSetName);
        H5::DataSpace nodesDSspace = nodesDS.getSpace();
        hsize_t *start = new hsize_t[nodesDSspace.getSimpleExtentNdims()]{static_cast<hsize_t>(index), 1};
        hsize_t *offset = new hsize_t[nodesDSspace.getSimpleExtentNdims()]{1, 1};
        nodesDSspace.selectHyperslab(H5S_SELECT_SET, start, offset);
        nodesDS.read(&node, IndexH5Type::NodeRawType(), nodesDSspace);
        delete[] start;
        delete[] offset;
    }
    catch (H5::FileIException e) {
        std::cerr << "ERROR: Cannot open file. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::DataSetIException e) {
        std::cerr << "ERROR: Cannot open dataset. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::GroupIException e) {
        std::cerr << "ERROR: Cannot open group. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::DataSpaceIException e) {
        std::cerr << "ERROR: Cannot open dataspace. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }

    return node;
}

EdgeRaw Routing::Hdf::DataIndexPart::GetEdge(int index) const {
    EdgeRaw edge;
    if (index > edgeCount - 1)
        std::cerr << "ERROR: Selected edge (index " << index << ") is outside of the dataset." << std::endl;

    try {
        // Open dataset
        H5::DataSet edgesDS = h5file.openGroup(Routing::Hdf::indexGroupPath + this->id_name).openDataSet(
                Routing::Hdf::edgeDataSetName);
        H5::DataSpace edgesDSspace = edgesDS.getSpace();
        hsize_t *start = new hsize_t[edgesDSspace.getSimpleExtentNdims()]{static_cast<hsize_t>(index), 1};
        hsize_t *offset = new hsize_t[edgesDSspace.getSimpleExtentNdims()]{1, 1};
        edgesDSspace.selectHyperslab(H5S_SELECT_SET, start, offset);
        edgesDS.read(&edge, IndexH5Type::EdgeRawType(), edgesDSspace);
        delete[] start;
        delete[] offset;
    }
    catch (H5::FileIException e) {
        std::cerr << "ERROR: Cannot open file. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::DataSetIException e) {
        std::cerr << "ERROR: Cannot open dataset. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::GroupIException e) {
        std::cerr << "ERROR: Cannot open group. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (H5::DataSpaceIException e) {
        std::cerr << "ERROR: Cannot open dataspace. " << e.getDetailMsg() << std::endl;
        exit(EXIT_FAILURE);
    }

    return edge;
}
