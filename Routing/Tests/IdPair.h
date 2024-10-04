#pragma once

#include <ostream>

namespace Routing {

    namespace Tests {

        class IdPair {

        public:
            std::string originalId;
            int localId;
            float latitude;
            float longitude;

            IdPair(std::string originalId, int localId, float latitude, float longitude) : originalId(originalId),
                                                                                           localId(localId),
                                                                                           latitude(latitude),
                                                                                           longitude(longitude) {}

            friend std::ostream &operator<<(std::ostream &os, const IdPair &id) {
                os << "OID: " << id.originalId << ", LID: " << id.localId << ", Lat: " << id.latitude <<
                   ", Lng: " << id.longitude;

                return os;
            }
        };
    }
}


