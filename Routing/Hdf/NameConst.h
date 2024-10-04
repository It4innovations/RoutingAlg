#pragma once

#include <string>

namespace Routing {

    namespace Hdf {
        static const std::string indexGroupPath = "/Index/";

        static const std::string indexCreateAttName = "CreationTime";
        static const std::string indexPartCountAttName = "PartsCount";
        static const std::string indexPartsAttName = "PartsInfo";

        static const std::string nodeMapDataSetName = "NodeMap";

        static const std::string nodeDataSetName = "Nodes";
        static const std::string edgeDataSetName = "Edges";
        static const std::string edgeDataDataSetName = "EdgeData";
    }
}
