
#include <iostream>
#include "RoutingIndexInfoCompType.h"
#include "../../Data/Structures/NodePosition.h"
#include "../../Data/Structures/EdgeRaw.h"
#include "../../Data/Structures/EdgeData.h"
#include "../../Data/Structures/NodeRaw.h"
#include "../../Data/Structures/RestrictedTurn.h"
#include "../../Data/Structures/ShortcutPosition.h"


H5::CompType Routing::IndexH5Type::RoutingIndexPartsCompType(void) {
    H5::CompType itype(sizeof(PartInfo));

    itype.insertMember("id", offsetof(PartInfo, idName), StringIdType());
    itype.insertMember("nodeCount", offsetof(PartInfo, nodeCount), H5::PredType::NATIVE_INT64);
    itype.insertMember("edgeCount", offsetof(PartInfo, edgeCount), H5::PredType::NATIVE_INT64);

    return itype;
}

H5::CompType Routing::IndexH5Type::NodeMapType() {
    H5::CompType itype(sizeof(Routing::NodePosition));

    itype.insertMember("nodeId", offsetof(Routing::NodePosition, nodeId), H5::PredType::NATIVE_INT32);
    itype.insertMember("partId", offsetof(Routing::NodePosition, partId), StringIdType());
    itype.insertMember("nodeIndex", offsetof(Routing::NodePosition, nodeIndex), H5::PredType::NATIVE_INT32);

    return itype;
}

H5::CompType Routing::IndexH5Type::EdgeRawType() {
    H5::CompType itype(sizeof(Routing::EdgeRaw));

    itype.insertMember("edgeId", offsetof(Routing::EdgeRaw, edgeId), H5::PredType::NATIVE_INT32);
    itype.insertMember("nodeIndex", offsetof(Routing::EdgeRaw, nodeIndex), H5::PredType::NATIVE_INT32);
    itype.insertMember("computed_speed", offsetof(Routing::EdgeRaw, computed_speed), H5::PredType::NATIVE_INT32);
    itype.insertMember("length", offsetof(Routing::EdgeRaw, length), H5::PredType::NATIVE_INT32);
    itype.insertMember("edgeDataIndex", offsetof(Routing::EdgeRaw, dataIndex),
                       H5::PredType::NATIVE_USHORT); // CAUTION: This type has to correspond with Definitions.h header!

    return itype;
}

H5::CompType Routing::IndexH5Type::EdgeDataType() {
    H5::CompType itype(sizeof(Routing::EdgeData));

    itype.insertMember("speed", offsetof(Routing::EdgeData, speed),
                       H5::PredType::NATIVE_FLOAT); // CAUTION: This type has to correspond with Definitions.h header!
    itype.insertMember("funcClass", offsetof(Routing::EdgeData, funcClass),
                       H5::PredType::NATIVE_UCHAR); // CAUTION: This type has to correspond with Definitions.h header!
    itype.insertMember("lanes", offsetof(Routing::EdgeData, lanes),
                       H5::PredType::NATIVE_UCHAR); // CAUTION: This type has to correspond with Definitions.h header!
    itype.insertMember("incline", offsetof(Routing::EdgeData, incline),
                       H5::PredType::NATIVE_SCHAR); // CAUTION: This type has to correspond with Definitions.h header!
    itype.insertMember("vehicleAccess", offsetof(Routing::EdgeData, vehicleAccess),
                       H5::PredType::NATIVE_UCHAR); // CAUTION: This type has to correspond with Definitions.h header!
    itype.insertMember("specificInfo", offsetof(Routing::EdgeData, specificInfo),
                       H5::PredType::NATIVE_UINT16); // CAUTION: This type has to correspond with Definitions.h header!

    itype.insertMember("maxWeight", offsetof(Routing::EdgeData, maxWeight),
                       H5::PredType::NATIVE_UINT16); // CAUTION: This type has to correspond with Definitions.h header!
    itype.insertMember("maxHeight", offsetof(Routing::EdgeData, maxHeight),
                       H5::PredType::NATIVE_UINT16); // CAUTION: This type has to correspond with Definitions.h header!
    itype.insertMember("maxAxleLoad", offsetof(Routing::EdgeData, maxAxleLoad),
                       H5::PredType::NATIVE_UCHAR); // CAUTION: This type has to correspond with Definitions.h header!
    itype.insertMember("maxWidth", offsetof(Routing::EdgeData, maxWidth),
                       H5::PredType::NATIVE_UCHAR); // CAUTION: This type has to correspond with Definitions.h header!
    itype.insertMember("maxLength", offsetof(Routing::EdgeData, maxLength),
                       H5::PredType::NATIVE_UCHAR); // CAUTION: This type has to correspond with Definitions.h header!

    return itype;
}

H5::CompType Routing::IndexH5Type::NodeRawType() {
    H5::CompType itype(sizeof(Routing::NodeRaw));

    itype.insertMember("id", offsetof(Routing::NodeRaw, id), H5::PredType::NATIVE_INT32);
    itype.insertMember("latitudeInt", offsetof(Routing::NodeRaw, latitudeInt), H5::PredType::NATIVE_INT32);
    itype.insertMember("longtitudeInt", offsetof(Routing::NodeRaw, longitudeInt), H5::PredType::NATIVE_INT32);
    itype.insertMember("edgeOutCount", offsetof(Routing::NodeRaw, edgesOutCount),
                       H5::PredType::NATIVE_UCHAR); // CAUTION: This type has to correspond with Definitions.h header!
    itype.insertMember("edgeOutIndex", offsetof(Routing::NodeRaw, edgesOutIndex), H5::PredType::NATIVE_INT32);
    itype.insertMember("edgeInCount", offsetof(Routing::NodeRaw, edgesInCount), H5::PredType::NATIVE_UCHAR);

    return itype;
}
