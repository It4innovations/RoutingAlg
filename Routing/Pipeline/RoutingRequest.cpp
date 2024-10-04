#include "RoutingRequest.h"
#include <fstream>
#include <sstream>

std::vector<Routing::Pipeline::RoutingRequest>
Routing::Pipeline::RoutingRequest::LoadRequests(const std::string file, int departureTime) {
    std::vector<RoutingRequest> requests;

    std::ifstream inputFile(file);
    std::string line;
    long lineNum = 1;

    while (inputFile.good()) {
        std::getline(inputFile, line);
        if (line.empty()) {
            continue;
        }

        size_t s = 0;
        auto subPos = [&line, &s]() -> size_t { return line.find(CSV_SEPARATOR, s); };
        auto getSubstr = [line](size_t start, size_t end) -> std::string { return line.substr(start, end); };

        size_t e = subPos();

        try {
            // 1st column
            unsigned long origin = std::stol(getSubstr(s, e - s));
            s = ++e;
            e = subPos();

            // 2nd column
            unsigned long destination = std::stol(getSubstr(s, e - s));
            s = ++e;
            e = subPos();

            requests.emplace_back(lineNum, origin, destination);
        }
        catch (std::out_of_range e) {
            std::cerr << "Cannot parse line (out of range) " << lineNum << std::endl;
            std::cerr << line << std::endl;
            std::cerr << e.what() << std::endl;
        }
        catch (std::invalid_argument e) {
            std::cerr << "Cannot parse line (invalid value) " << lineNum << std::endl;
            std::cerr << line << std::endl;
            std::cerr << e.what() << std::endl;
        }

        lineNum++;
    }

    inputFile.close();
#ifndef NDEBUG
    std::cout << "Loaded " << requests.size() << " requests." << std::endl;
#endif

    return requests;
}
