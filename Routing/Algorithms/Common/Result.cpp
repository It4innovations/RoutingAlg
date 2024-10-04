
#include "Result.h"

std::unique_ptr<Routing::Algorithms::Result> Routing::Algorithms::Result::Take(int n) {
    if (n <= 0) return nullptr;

    auto rs = std::vector<Segment>(this->results->begin(), this->results->end());

    std::unique_ptr<Routing::Algorithms::Result> ret(new Result(rs,
                                                                static_cast<float>((*this->results)[n -
                                                                                                    1].time),
                                                                (*this->results)[n - 1].length,
                                                                long(0)));

    return ret;
}

bool Routing::Algorithms::Result::StartsWith(const Routing::Algorithms::Result &other) {
    return std::equal(this->results->begin(), this->results->end(), other.results->begin(),
                      [](const Segment &s1, const Segment &s2) -> bool {
                          return s1.edgeId == s2.edgeId;
                      });
}

std::unique_ptr<Routing::Algorithms::Result> Routing::Algorithms::Result::Add(
        Result &r1, Result &r2) {

    std::unique_ptr<Result> result(new Result(
            r1.computingTime + r2.computingTime
    ));

    result->results.reset(new std::vector<Segment>);
    result->results->reserve(r1.results->size() + r2.results->size());
    result->results->insert(result->results->end(), r1.results->begin(), r1.results->end());

    int r1LastLen = r1.results->back().length;
    float r1LastTim = r1.results->back().time;

    for (unsigned int i = 0; i < r2.results->size(); ++i) {
        Segment &tmp = (*r2.results)[i];
        tmp.time = r1LastTim;
        tmp.length = r1LastLen;

        result->results->push_back(tmp);
    }

    result->travelLength = result->results->back().length;
    result->travelTime = result->results->back().time;

    return result;
}

void Routing::Algorithms::Result::CSVserialize(const std::string &savePath) {
    std::ofstream resFile(savePath);

    for (auto &res: *this->results) {
        resFile << res.nodeId1 << " " <<
                res.nodeId2 << " " <<
                res.edgeId << " " <<
                res.length << " " <<
                res.time << " " <<
                res.geometry.get()->at(0).lat << " " <<
                res.geometry.get()->at(0).lon << " " <<
                res.geometry.get()->back().lat << " " <<
                res.geometry.get()->back().lon << "\n";
    }

    resFile.close();
}

bool Routing::Algorithms::Result::CSVcompare_old(const std::string &diffPath) {
    std::ifstream cmpFile(diffPath);

    std::vector<Segment> cmpVec;
    cmpVec.reserve(this->results->size());

    unsigned int i = 0;
    int r_edge, r_node1, r_node2, r_len;

    float r_time = 0.0;

    std::string line;

    while (cmpFile.good()) {
        std::getline(cmpFile, line);

        this->SplitByDelimiter(line, ';', r_node1, r_node2, r_edge, r_len, r_time);

        std::vector<GpsPoint> geo(2);
        Segment cmp(r_edge, r_node1, r_node2, r_len, r_time, 0, geo);

        Segment tmp = (*this->results)[i];

        if (tmp == cmp) {
        } else {
            std::cerr << "Index: " << i << " not match: \n" << tmp << "\n\n" << cmp << std::endl;
            return false;
        }

        if (cmpFile.eof()) {
            if (i + 1 != this->results->size()) {
                std::cout << "Invalid csv result size: have " << i + 1 <<
                          " - want " << this->results->size() << std::endl;

                return false;
            } else {
                std::cout << "EOF - " << i + 1 << " records" << std::endl;
            }

            break;
        }

        i++;
    }

    return true;
}

void Routing::Algorithms::Result::SplitByDelimiter(std::string &line, char delimiter,
                                                   int &node1, int &node2, int &edge, int &len, float &time) {

    int s = 0;
    auto subPos = [&line, delimiter, &s]() -> unsigned long { return line.find(delimiter, s); };
    auto getSubstr = [line](int s, int end) -> std::string { return line.substr(s, end); };


    int e = subPos();

    node1 = std::stoi(getSubstr(s, e - s).c_str());
    s = ++e;

    e = subPos();
    node2 = std::stoi(getSubstr(s, e - s).c_str());
    s = ++e;

    e = subPos();
    edge = std::stoi(getSubstr(s, e - s).c_str());
    s = ++e;

    e = subPos();
    len = std::stoi(getSubstr(s, e - s));
    s = ++e;

    time = std::stof(getSubstr(s, line.length()).c_str());
}


Routing::Algorithms::Result::Result(int computingTime) : travelTime(0),
                                                         travelLength(0),
                                                         computingTime(computingTime) {}

Routing::Algorithms::Result::Result(std::vector<Segment> &results, float time, int length, int computingTime)
        : travelTime(time),
          travelLength(length),
          computingTime(computingTime),
          results(new std::vector<Segment>(results)) {}

Routing::Algorithms::Result::Result(std::vector<Segment> &results, float time, int length, long computingTime)
        : travelTime(time),
          travelLength(length),
          computingTime(static_cast<int>(computingTime)),
          results(new std::vector<Segment>(results)) {}

std::vector<Routing::Algorithms::Segment> Routing::Algorithms::Result::GetResult() {
    return *results;
}

const std::vector<Routing::Algorithms::Segment> &Routing::Algorithms::Result::GetResult() const {
  return *results;
}
