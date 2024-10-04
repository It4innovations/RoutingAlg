#pragma once

namespace Routing {

    namespace Algorithms {

        struct FilterSettings {
            bool allFilterOff = false;
            bool hierarchyStartPositionFilterOn = true;
            int minFRC = 7;
            int maxFRC = 0;
            int firstHierarchyJumpFRC = 5;
            double firstHierarchyJumpDistance = 15;
            double firstHierarchyJumpLongDistance = 5;
            int secondHierarchyJumpFRC = 3;
            double secondHierarchyJumpDistance = 80;
            double secondHierarchyJumpLongDistance = 20;
            double longDistanceValue = 120;
            GraphFilterGeometry::FilterType typeOfFilter = GraphFilterGeometry::FilterType::Ellipse;
        };
    }
}
