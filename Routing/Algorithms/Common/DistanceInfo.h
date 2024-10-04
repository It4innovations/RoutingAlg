#pragma once

#include <vector>
#include <memory>
#include <ostream>
#include <algorithm>
#include <fstream>
#include <functional>

namespace Routing {

    namespace Algorithms {

        class DistanceInfo {
        public:
            int NodeId1;
            int NodeId2;
            float TravelTime;
            int TravelLength;

            DistanceInfo(int nodeId1, int nodeId2, float travelTime, int travelLength);

            DistanceInfo(std::string nodeId1, std::string nodeId2, float travelTime, int travelLength);

            DistanceInfo(const DistanceInfo &other);

            float GetAverageSpeed() const;

            friend std::ostream &operator<<(std::ostream &os, const DistanceInfo &res) {
                os << "NID1: " << res.NodeId1 << ", NID2: " <<
                   res.NodeId2 << ", Travel length: " << res.TravelLength <<
                   ", TravelTime: " << res.TravelTime;

                return os;
            }

            bool operator==(const DistanceInfo &other) const {
                return other.NodeId1 == NodeId1 && other.NodeId2 == NodeId2 &&
                       other.TravelLength == TravelLength && other.TravelTime == TravelTime;
            }

            bool operator<(const DistanceInfo &other) const {
                return (NodeId1 < other.NodeId1) || (NodeId1 == other.NodeId1 && NodeId2 < other.NodeId2);
            }
        };
    }
}

//namespace std {
//    template<>
//    struct hash<Routing::Algorithms::DistanceInfo> {
//        typedef Routing::Algorithms::DistanceInfo argument_type;
//        typedef std::size_t result_type;
//
//        result_type operator()(argument_type const &s) const {
//            return std::hash<int>{}(s.NodeId1) ^ std::hash<int>{}(s.NodeId2 << 7)
//                   ^ std::hash<float>{}(s.TravelTime) ^ std::hash<int>{}(s.TravelLength << 13);
//        }
//    };
//}
