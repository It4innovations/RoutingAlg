#pragma once

#include <H5Cpp.h>
#include "../RoutingIndexInfo.h"

namespace Routing {

    namespace IndexH5Type {
        constexpr size_t ID_TYPE_SZ = sizeof(char) * ID_INFO_NAME_SZ + 1;

        inline H5::PredType RoutingIndexCreateType() { return H5::PredType::NATIVE_INT64; };

        inline H5::PredType RoutingIndexCountType() { return H5::PredType::NATIVE_INT32; };


        inline H5::StrType StringIdType() {
            return H5::StrType(H5::PredType::C_S1, ID_TYPE_SZ);
        };

        H5::CompType RoutingIndexPartsCompType();

        H5::CompType RoutingIndexInfoPartCompType();

        H5::CompType NodePositionType();

        H5::CompType EdgeRawType();

        H5::CompType EdgeDataType();

        H5::CompType NodeRawType();

        H5::CompType NodeMapType();
    }
}




