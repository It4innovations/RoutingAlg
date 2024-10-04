#ifndef ROUTINGINDEXINFO_H_
#define ROUTINGINDEXINFO_H_

#include <vector>
#include <H5Cpp.h>

#define ID_INFO_NAME_SZ 3

namespace Routing {

    struct PartInfo {
        char idName[ID_INFO_NAME_SZ];
        long nodeCount;
        long edgeCount;
    };
}
#endif
