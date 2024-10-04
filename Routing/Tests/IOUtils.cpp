
#include "IOUtils.h"


#include <sstream>

std::shared_ptr<GraphMemory> Tests::LoadGraph(const std::string &hdfIndexPath) {
#ifndef NDEBUG
    cout << "Creating data index file" << endl;
#endif
    H5::H5File h5file(hdfIndexPath, H5F_ACC_RDONLY);
    unique_ptr<DataIndex> dif(new DataIndex(h5file));
    h5file.close();

#ifndef NDEBUG
    cout << "Creating routing graph" << endl;
#endif

    shared_ptr<GraphMemory> graph(new GraphMemory(*dif));

    return graph;
}

std::vector<std::tuple<int, int>> Routing::Tests::LoadIdQuerySet(const std::string &queryFilePath) {
    std::ifstream cmpFile(queryFilePath);

    std::string line;

    std::vector<std::tuple<int, int>> querySet;

    while (cmpFile.good()) {
        std::getline(cmpFile, line);

        if (line.empty())
            break;

        auto query = ParseIdQuery(line);

        querySet.push_back(query);
    }

    cmpFile.close();

    return querySet;
}

std::vector<GpsPoint> Routing::Tests::LoadGpsSet(const std::string &queryFilePath, int gpsOffset) {
    std::ifstream cmpFile(queryFilePath);

    std::string line;

    std::vector<GpsPoint> gpsSet;

    while (cmpFile.good()) {
        std::getline(cmpFile, line);

        cout << line << endl;

        if (line.empty())
            break;

        auto gps = ParseGps(line, gpsOffset);

        gpsSet.push_back(gps);
    }

    cmpFile.close();

    return gpsSet;
}


std::vector<Routing::Tests::IdPair> Routing::Tests::LoadIdGpsSet(const std::string &queryFilePath) {
    const char SEPARATOR = ';';
    // Input file format:
    // SetId;OriginalId;Lat;Lng;...skip...;LocalId

    std::ifstream cmpFile(queryFilePath);
    std::string line;
    std::vector<IdPair> result;

    while (cmpFile.good()) {
        std::getline(cmpFile, line);

        if (line.empty())
            break;

        size_t s = 0;
        auto subPos = [&line, &s]() -> size_t { return line.find(SEPARATOR, s); };
        auto getSubstr = [line](size_t start, size_t end) -> std::string { return line.substr(start, end); };

        size_t e = subPos();

        std::string setId = getSubstr(s, e - s);
        s = ++e;
        e = subPos();

        std::string originalId = getSubstr(s, e - s);
        s = ++e;
        e = subPos();

        float lng = std::stof(getSubstr(s, e - s));
        s = ++e;
        e = subPos();

        float lat = std::stof(getSubstr(s, e - s));
        s = ++e;

        size_t last_s = line.find_last_of(SEPARATOR);
        long localId = std::stol(getSubstr(++last_s, line.length()));

        result.push_back(IdPair(originalId, localId, lat, lng));
    }

    cmpFile.close();
    return result;
}

std::tuple<int, int> Routing::Tests::ParseIdQuery(const std::string &queryString) {

    size_t s = 0;
    auto subPos = [&queryString, &s]() -> size_t { return queryString.find(';', s); };
    auto getSubstr = [&queryString](size_t start, size_t end) -> std::string { return queryString.substr(start, end); };

    size_t e = subPos();

    int node1 = std::stoi(getSubstr(s, e - s).c_str());
    s = ++e;
    int node2 = std::stoi(getSubstr(s, queryString.length()).c_str());

    return make_tuple(node1, node2);
}

GpsPoint Routing::Tests::ParseGps(const std::string &queryString, int gpsOffset) {

    size_t s = 0;
    for (int i = 0; i < gpsOffset; ++i) {
        s = queryString.find(';', s) + 1;
    }

    auto subPos = [&queryString, &s]() -> size_t { return queryString.find(';', s); };
    auto getSubstr = [queryString](size_t start, size_t end) -> std::string { return queryString.substr(start, end); };

    size_t e = subPos();

    float lat = std::stof(getSubstr(s, e - s).c_str());
    s = ++e;
    e = subPos();
    float lng = s != string::npos ? std::stof(getSubstr(s, e - s).c_str()) :
                std::stof(getSubstr(s, queryString.length()).c_str());

    return GpsPoint(lat, lng);
}

std::tuple<GpsPoint, GpsPoint> Routing::Tests::ParseGpsQuery(const std::string &queryString, int gpsOffset) {

    size_t s = 0;
    for (int i = 0; i < gpsOffset; ++i) {
        s = queryString.find(';', s) + 1;
    }

    auto subPos = [&queryString, &s]() -> size_t { return queryString.find(';', s); };
    auto getSubstr = [queryString](size_t start, size_t end) -> std::string { return queryString.substr(start, end); };

    size_t e = subPos();

    float lat1 = std::stof(getSubstr(s, e - s).c_str());
    s = ++e;
    e = subPos();
    float lng1 = std::stof(getSubstr(s, e - s).c_str());
    s = ++e;
    e = subPos();
    float lat2 = std::stof(getSubstr(s, e - s).c_str());
    s = ++e;
    float lng2 = std::stof(getSubstr(s, queryString.length()).c_str());

    return make_tuple(GpsPoint(lat1, lng1), GpsPoint(lat2, lng2));
}

std::vector<tuple<GpsPoint, GpsPoint>> Routing::Tests::LoadGpsQuerySet(const string &queryFilePath, int gpsOffset) {
    std::ifstream cmpFile(queryFilePath);

    std::string line;

    std::vector<std::tuple<GpsPoint, GpsPoint>> querySet;

    while (cmpFile.good()) {
        std::getline(cmpFile, line);

        if (line.empty())
            break;


        auto query = ParseGpsQuery(line, gpsOffset);

        querySet.push_back(query);
    }

    cmpFile.close();

    return querySet;
}


std::vector<Routing::Algorithms::Segment> Routing::Tests::LoadResult(const std::string &resultPath) {
    std::ifstream resultFile(resultPath);

    std::vector<Routing::Algorithms::Segment> segments;

    std::string line;

    while (resultFile.good()) {
        std::getline(resultFile, line);

        if (line.empty()) {
            break;
        }

        Routing::Algorithms::Segment segment = ParseResultFile(line);

        segments.push_back(segment);

    }

    resultFile.close();

    return segments;
}

std::vector<int> Routing::Tests::LoadTestResult(const std::string &resultPath) {
    std::ifstream resultFile(resultPath);

    std::vector<int> edges;

    std::string line;

    while (resultFile.good()) {
        std::getline(resultFile, line);

        if (line.empty()) {
            break;
        }

        auto edge = ParseTestLine(line);

        edges.push_back(edge);

    }

    resultFile.close();

    return edges;
}

Routing::Algorithms::Segment Routing::Tests::ParseResultFile(string line) { //Get one segment from the file
    //[Node1, Node2, Edge, long of path, time, lat Node1, lon Node1, lat Node2, lon Node2]
    string tokens[9];
    int fromIndex = 0;

    for (int i = 0; i < 9; i++) {
        std::string delimiter = " ";
        int toIndex = line.find(delimiter, fromIndex);
        std::string token = line.substr(fromIndex, toIndex - fromIndex);
        fromIndex = toIndex + 1;
        tokens[i] = token;
    }

    Routing::GpsPoint node1Coordinates(std::stof(tokens[5]), std::stof(tokens[6])); //Node 1 GPS coordinates
    Routing::GpsPoint node2Coordinates(std::stof(tokens[7]), std::stof(tokens[8])); //Node 2 GPS coordinates

    std::vector<Routing::GpsPoint> geometry;

    geometry.push_back(node1Coordinates);
    geometry.push_back(node2Coordinates);

    Routing::Algorithms::Segment segment(std::stoi(tokens[2]), std::stoi(tokens[0]), std::stoi(tokens[1]),
                                         std::stoi(tokens[3]),
                                         std::stof(tokens[4]), true, geometry);

    return segment;
}

int Routing::Tests::ParseTestLine(string line) {

    int fromIndex = 0;
    int tokensCount = 1;

    string tokens[tokensCount];

    for (int i = 0; i < tokensCount; i++) {
        std::string delimiter = " ";
        int toIndex = line.find(delimiter, fromIndex);
        std::string token = line.substr(fromIndex, toIndex - fromIndex);
        fromIndex = toIndex + 1;
        tokens[i] = token;
    }

    return std::stoi(tokens[0]);

}

std::vector<Routing::Tests::IdPair> Routing::Tests::RemoveNonExistentNodes(std::vector<Routing::Tests::IdPair> &idPairs,
                                                                           const shared_ptr<GraphMemory> routingGraph) {

    std::vector<IdPair> notFoundPairs;
    std::vector<IdPair> foundPairs;
    for (const auto &pair : idPairs) {
        if (!routingGraph->ContainsNode(pair.localId)) {
            notFoundPairs.push_back(pair);
            continue;
        }
        foundPairs.push_back(pair);
    }

    if (notFoundPairs.size() > 0) {
        for (const auto &pair : notFoundPairs) {
            std::cerr << "Node " << pair.localId << " not found." << std::endl;
        }
    }
    std::cerr << "Removed " << notFoundPairs.size() << " nodes" << std::endl;

    return foundPairs;
}

