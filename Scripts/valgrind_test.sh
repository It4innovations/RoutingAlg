#!/usr/bin/env bash

#valgrind --log-file=valgrind.log --gen-suppressions=all -v /home/tomhet/.CLion2016.1/system/cmake/generated/StaticRoutingCPP-b032ea0a/b032ea0a/Debug/Routing /home/tomhet/ClionProjects/StaticRoutingData/index_16_04_19/osm_hdf/index.h5

valgrind --log-file=valgrind.log --leak-check=yes -v /home/tomhet/.CLion2016.1/system/cmake/generated/StaticRoutingCPP-b032ea0a/b032ea0a/Debug/Routing /home/tomhet/ClionProjects/StaticRoutingData/index_16_04_19/osm_hdf/index.h5
