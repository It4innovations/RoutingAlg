
#include "GraphPartMemory.h"

Routing::Data::GraphPartMemory::GraphPartMemory(const std::vector<Routing::Node> &nodes,
                                                const std::vector<Routing::Edge> &edges,
                                                const Routing::GpsRectangle &boundingBox) {
    for (const auto &node: nodes) {
        this->nodes.emplace_back(node);
    }

    for (const auto &edge: edges) {
        this->edges.emplace_back(edge);
    }

    this->boundingBox = boundingBox;
}


Routing::Data::GraphPartMemory::GraphPartMemory(const vector<Routing::NodeRaw> &nodesRaw,
                                                const vector<Routing::EdgeRaw> &edgesRaw,
                                                const std::vector<EdgeData> &edgeData,
                                                Routing::GpsRectangle &&boundingBox) :
        boundingBox(boundingBox) {
    auto size = edgeData.size();
    for (const EdgeRaw &e: edgesRaw) {
        this->edges.emplace_back(e.edgeId, e.computed_speed, e.length, &edgeData[e.dataIndex], e.nodeIndex);
    }

    //TODO diff NodeRaw vs Node
    //TODO refactoring
    for (const NodeRaw &n : nodesRaw) {

        std::vector<Edge *> edgeOut;
        edgeOut.reserve(n.edgesOutCount);

        int maxIndex = n.edgesOutIndex + n.edgesOutCount;

        for (int i = n.edgesOutIndex; i < maxIndex; ++i) {
            edgeOut.push_back(&this->edges[i]);
        }

        this->nodes.emplace_back(Node(n.id, std::move(edgeOut), n.latitudeInt, n.longitudeInt, n.edgesInCount));
    }

    for (auto &nd: this->nodes) {
        for (Edge *ed: nd.edgesOut) {
            ed->startNodePtr = &nd;
        }
    }

    for (auto &&ed : this->edges) {
        ed.endNode.endNodePtr = &this->nodes[ed.endNode.endNodeIndex];
        ed.endNode.endNodePtr->edgesIn.emplace_back(&ed);
    }
}

const Routing::GpsRectangle &Routing::Data::GraphPartMemory::GetBoundingBox(void) const {
    return this->boundingBox;
}

const Routing::Node *Routing::Data::GraphPartMemory::GetNodePtr(int nodePos) const {
    return &this->nodes[nodePos];
}

const Routing::Edge *Routing::Data::GraphPartMemory::GetEdgePtr(int edgePos) const {
    return &this->edges[edgePos];
}

unsigned long Routing::Data::GraphPartMemory::GetNodeCount(void) {
    return this->nodes.size();
}

unsigned long Routing::Data::GraphPartMemory::GetEdgeCount(void) {
    return this->edges.size();
}

void Routing::Data::GraphPartMemory::PrintEdges() {
    for (const Edge &e: edges) {
        cout << e << endl;
    }
}

void Routing::Data::GraphPartMemory::PrintNodes() {
    for (const Node &n: nodes) {
        cout << n << endl;
    }
}

void Routing::Data::GraphPartMemory::ResetSpeed() {
    for (auto &&e: this->edges) {
        e.ResetSpeed();
    }
}
