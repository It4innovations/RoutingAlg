#pragma once

#include <type_traits>

#include "../Utility/Definitions.h"

//using Routing::byte;

namespace Routing {
    enum class ObjectType : unsigned char {
        Node = 1,
        Edge = 2,
        Geometry = 4,
        Unknown = Node | Edge | Geometry


    };
}

inline std::ostream &operator<<(std::ostream &os, Routing::ObjectType &type) {
    os << type;
    return os;
}

inline Routing::ObjectType operator|(Routing::ObjectType lhs, Routing::ObjectType rhs) {
    return (Routing::ObjectType) (static_cast<std::underlying_type<Routing::ObjectType>::type>(lhs) |
                                  static_cast<std::underlying_type<Routing::ObjectType>::type>(rhs));
}

inline Routing::ObjectType &operator|=(Routing::ObjectType &lhs, Routing::ObjectType rhs) {
    lhs = (Routing::ObjectType) (static_cast<std::underlying_type<Routing::ObjectType>::type>(lhs) |
                                 static_cast<std::underlying_type<Routing::ObjectType>::type>(rhs));
    return lhs;
}

inline Routing::ObjectType operator&(Routing::ObjectType lhs, Routing::ObjectType rhs) {
    return (Routing::ObjectType) (static_cast<std::underlying_type<Routing::ObjectType>::type>(lhs) &
                                  static_cast<std::underlying_type<Routing::ObjectType>::type>(rhs));
}
