#pragma once

namespace Routing {

    namespace Hdf {
        enum class ELoadType {
            Graph = 0,
            TurnRestrictions = 1,
            GeometryShortcuts = 2,
            PathShortcuts = 4,
            Shortcuts = ELoadType::GeometryShortcuts | ELoadType::PathShortcuts,
            Restrictions = ELoadType::TurnRestrictions,
            All = ELoadType::Graph | ELoadType::Restrictions | ELoadType::Shortcuts
        };

        inline ELoadType operator|(ELoadType a, ELoadType b) {
            return static_cast<ELoadType>(static_cast<int>(a) | static_cast<int>(b));
        }

        inline ELoadType operator&(ELoadType a, ELoadType b) {
            return static_cast<ELoadType>(static_cast<int>(a) & static_cast<int>(b));
        }


    }
}


