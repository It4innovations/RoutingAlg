#pragma once

#include <type_traits>
#include <ostream>

namespace Routing {
    enum ESpecificInfo : uint16_t {
        None = 0,
        TollWay = 1,
        SlipRoad = 2,
        MultiCarriageway = 4,
        Roundabout = 8,
        BorderCross = 16,
        Bridge = 32,
        Tunnel = 64
    };
}

inline Routing::ESpecificInfo operator|(Routing::ESpecificInfo lhs, Routing::ESpecificInfo rhs) {
    return (Routing::ESpecificInfo) (static_cast<std::underlying_type<Routing::ESpecificInfo>::type>(lhs) |
                                     static_cast<std::underlying_type<Routing::ESpecificInfo>::type>(rhs));
}

inline Routing::ESpecificInfo &operator|=(Routing::ESpecificInfo &lhs, Routing::ESpecificInfo rhs) {
    lhs = (Routing::ESpecificInfo) (static_cast<std::underlying_type<Routing::ESpecificInfo>::type>(lhs) |
                                    static_cast<std::underlying_type<Routing::ESpecificInfo>::type>(rhs));
    return lhs;
}

inline Routing::ESpecificInfo operator&(Routing::ESpecificInfo lhs, Routing::ESpecificInfo rhs) {
    return (Routing::ESpecificInfo) (static_cast<std::underlying_type<Routing::ESpecificInfo>::type>(lhs) &
                                     static_cast<std::underlying_type<Routing::ESpecificInfo>::type>(rhs));
}
