#pragma once

#include <vector>
#include <ostream>
#include <fstream>
#include <sstream>
#include <cstring>

#include "Segment.h"

namespace Routing {

    namespace Algorithms {

        class Result {
        public:
            float travelTime;
            int travelLength;
            int computingTime;

            Result(int computingTime);

            Result(std::vector<Segment> &results, float time, int length, int computingTime);

            Result(std::vector<Segment> &results, float time, int length, long computingTime);


            Result(Result &&result) {
                this->travelTime = result.travelTime;
                result.travelTime = 0;
                this->travelLength = result.travelLength;
                result.travelLength = 0;
                this->computingTime = result.computingTime;
                result.computingTime = 0;
                this->results.reset(result.results.release());
            }


            friend std::ostream &operator<<(std::ostream &os, const Result &res) {
                os << "Segments: " << res.results->size() << ", Travel time: " <<
                   res.travelTime << ", Travel length: " << res.travelLength <<
                   ", Computing time: " << res.computingTime << std::endl;

                int i = 0;
                for (auto &&  sg : *res.results) {
                    os << sg << std::endl;
                    i++;
                    if (i >= 5)
                        break;
                }

                return os;
            }

            void CSVRoutes(std::ofstream &os){
                for (auto &&  sg : *results) {
                    os << sg.nodeId1 << ";";
                }

                if (!results->empty()){
                    int last_id = results->back().nodeId2;
                    os << last_id << "\n";
                }
            }

            void CSVserialize(const std::string &savePath);

            std::vector<Segment> GetResult();
            [[nodiscard]] const std::vector<Segment>& GetResult() const;

        private:
            std::unique_ptr<std::vector<Segment>> results;

            void SplitByDelimiter(std::string &line, char delimiter, int &node1, int &node2, int &edge, int &len,
                                  float &time);

            std::unique_ptr<Result> Take(int n);

            bool StartsWith(const Result &other);

            static std::unique_ptr<Result> Add(Result &r1, Result &r2);

            bool CSVcompare_old(const std::string &diffPath);
        };
    }
}


