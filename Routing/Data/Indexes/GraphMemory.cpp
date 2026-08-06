
#include "GraphMemory.h"
#include <iomanip>


Routing::Data::GraphMemory::GraphMemory(DataIndex &indexData) :
        edgeData(indexData.GetEdgeData()),
        boundingBox(GpsRectangle()){

    TimeWatch watch(true);

    this->info = GraphInfo(indexData.GetTimeCreated(),
                           indexData.GetIndexPartsCount());

    time_t ctime = this->info.GetTimeCreated();
    struct tm lctime = *std::localtime(&ctime);
    char buf[256];
    std::strftime(buf, 256, "%H:%M:%S %d.%m.%Y", &lctime);

#ifndef NDEBUG
    std::cout << "Index built at: " << buf << std::endl;
    std::cout << "Creating routing index: " << this->info.GetPartCount() << " parts" << std::endl;
#endif

    for (int i = 0; i < this->info.GetPartCount(); ++i) {

#ifndef NDEBUG
        std::cout << "Processing " << i + 1 << ". part\n";
#endif
        std::string partId = indexData.GetPartInfo()[i].idName;
        DataIndexPart *part = indexData.GetIndexPart(i);

        assert(part != nullptr);

        std::vector<NodeRaw> nodesRaw = part->GetNodes();
        std::vector<EdgeRaw> edgesRaw = part->GetEdges();

#ifndef NDEBUG
        std::cout << "Creating graph memory " << i + 1 << std::endl;
#endif

        std::shared_ptr<GraphPartMemory> ip(new GraphPartMemory(nodesRaw,
                                                                edgesRaw,
                                                                this->edgeData,
                                                                GpsRectangle(part->GetLeft(),
                                                                             part->GetTop(),
                                                                             part->GetRight(),
                                                                             part->GetBottom())));

        assert(ip != nullptr);

        this->parts[partId] = ip;

        const GpsRectangle &rec = this->parts[partId]->GetBoundingBox();

        this->boundingBox.Expand(rec);
    }

    this->nodeIdStore.set_empty_key(0);
    this->edgeIdStore.set_empty_key(0);

    for (auto &&nd: indexData.GetNodeMap()) {
        this->nodeIdStore[nd.nodeId] =  this->parts[nd.partId]->GetNodePtr(nd.nodeIndex);

        for (auto &&e: this->nodeIdStore[nd.nodeId]->GetEdgesOut()) {
            this->edgeIdStore[e->edgeId] = e;
        }
    }

    watch.Stop();


#ifndef NDEBUG
    std::cout << "Memory routing graph loaded in " << watch.GetElapsed() << " ms. IdMap size: "
              << this->nodeIdStore.size() << ". Time created: " << this->info.GetTimeCreated() << std::endl;
#endif
}

Routing::Data::GraphMemory::~GraphMemory() {
    this->nodeIdStore.clear();
}

bool Routing::Data::GraphMemory::ContainsNode(int nodeId) const {
    return this->nodeIdStore.find(nodeId) != this->nodeIdStore.end();
}

const Routing::Node &Routing::Data::GraphMemory::GetNodeById(int nodeId) const {
    const auto node = this->nodeIdStore.find(nodeId);
    if (node == this->nodeIdStore.end() || node->second == nullptr) {
        throw NodeNotFoundException(nodeId);
    }
    return *node->second;
}

const Routing::Edge &Routing::Data::GraphMemory::GetEdgeById(int edgeId) const {
    const auto edge = this->edgeIdStore.find(edgeId);
    if (edge == this->edgeIdStore.end() || edge->second == nullptr) {
        throw EdgeNotFoundException(edgeId);
    }
    return *edge->second;
}

void Routing::Data::GraphMemory::SetEdgeSpeed(int edgeId, float speed) {
    const auto edge = this->edgeIdStore.find(edgeId);
    if (edge == this->edgeIdStore.end() || edge->second == nullptr) {
        throw EdgeNotFoundException(edgeId);
    }
    const_cast<Edge *>(edge->second)->SetSpeed(speed);
}

const std::vector<Routing::Edge *> Routing::Data::GraphMemory::GetEdgesIn(int nodeId) const {
    return this->GetNodeById(nodeId).GetEdgesIn();
}

const std::vector<Routing::Edge *> Routing::Data::GraphMemory::GetEdgesOut(int nodeId) const {
    return this->GetNodeById(nodeId).GetEdgesOut();
}

const Routing::Node &Routing::Data::GraphMemory::GetEndNodeByEdge(const Edge &edge) const {
    return *edge.endNode.endNodePtr;
}

const Routing::Node &Routing::Data::GraphMemory::GetStartNodeByEdge(const Edge &edge) const {
    return *edge.startNodePtr;
}

int Routing::Data::GraphMemory::GetNodeCount() {
    int count = 0;

    for (auto const &part: this->parts) {
        count += part.second->GetNodeCount();
    }

    return count;
}

int Routing::Data::GraphMemory::GetEdgeCount() {
    int count = 0;

    for (auto const &part: this->parts) {
        count += part.second->GetEdgeCount();
    }

    return count;
}

const Routing::GpsRectangle &Routing::Data::GraphMemory::GetBoundingBox(void) {
    return this->boundingBox;
}

std::vector<Routing::GpsRectangle> Routing::Data::GraphMemory::GetBoundingBoxes(void) {
    std::vector<GpsRectangle> ret;
    ret.reserve(this->parts.size());

    for (auto const &part: this->parts) {
        ret.push_back(part.second->GetBoundingBox());
    }

    return ret;
}

const std::vector<Routing::Edge *> Routing::Data::GraphMemory::GetEdgesOut(const Routing::Node &node) const {
    return node.GetEdgesOut();
}

void Routing::Data::GraphMemory::ResetGraphSpeed() {
    for (auto &&part  : this->parts) {
        part.second->ResetSpeed();
    }
}

Segment Routing::Data::GraphMemory::EdgeToSegment(int edgeId) {
    return this->EdgeToSegment(this->GetEdgeById(edgeId));
}

Segment Routing::Data::GraphMemory::EdgeToSegment(const Routing::Edge &edge) {
    // Create segment
    auto segment = Segment(edge.edgeId, edge.startNodePtr->id,
                           edge.endNode.endNodePtr->id, edge.length);

    // Compute time needed to pass the segment from computed_speed, which is stored in Edge::speed attribute, which is accessed by GetSpeed() getter O_o
    // TODO: Get rid of this abomintaion once for all
    segment.time = edge.length / edge.GetSpeed();
    segment.speed = edge.GetSpeed();
    return segment;
}
