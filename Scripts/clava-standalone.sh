#!/usr/bin/env bash

CMAKE_SOURCE_DIR="/Users/gol0011/IT4I/Routing/StaticRoutingCPP"
INPUT_FILES="/Users/gol0011/IT4I/Routing/StaticRoutingCPP/Routing/Hdf/RoutingIndexInfo.h:/Users/gol0011/IT4I/Routing/StaticRoutingCPP/Routing/Data/Structures/NodePosition.h:/Users/gol0011/IT4I/Routing/StaticRoutingCPP/Routing/Data/Structures/EdgeRaw.h:/Users/gol0011/IT4I/Routing/StaticRoutingCPP/Routing/Data/Structures/EdgeData.h:/Users/gol0011/IT4I/Routing/StaticRoutingCPP/Routing/Data/Structures/NodeRaw.h:/Users/gol0011/IT4I/Routing/StaticRoutingCPP/Routing/Data/Structures/RestrictedTurn.h:/Users/gol0011/IT4I/Routing/StaticRoutingCPP/Routing/Data/Structures/ShortcutPosition.h"

java -Djava.awt.headless=true -jar /Users/gol0011/IT4I/clava/clava.jar ${CMAKE_SOURCE_DIR}/Aspects/hdf5.lara \
    -p ${INPUT_FILES} \
    -i ${CMAKE_SOURCE_DIR}/Aspects \
    --flags "-ffreestanding"