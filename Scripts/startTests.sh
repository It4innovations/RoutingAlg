#!/usr/bin/env bash

#c kompilator
#v debug/release
#a typ algoritmu
#r vyhodnoceni
#i vstupni soubor s id
#5 hdfpath - bude natvrdo

module purge

COMPILER= #"intel"
BUILD_TYPE= #"Release"
PROFILER="gprof"
INPUT_ID= #"~/RoutinGit/StaticRoutingData/index_16_04_19/query_sets/10_random_ids.csv"
HDF_PATH="~/RoutingGit/StaticRoutingData/index_16_04_19/osm_hdf/index.h5"

ADD_RUN_PARAMS=""

ROUTING_MAIN_PATH="~/RoutingGit/StaticRoutingCPP/"
RESULT_PATH="~/RoutingGit/results/"

RELEASE_BUILD_PATH="Release/"
DEBUG_BUILD_PATH="Debug/"
COMPILE_PATH=""
BINARY_NAME="Routing"

set_gcc_compile(){
    echo "GCC compiler"
    module load CMake/3.3.2-GNU-4.9.3-2.25
}

set_intel_compiler(){
    echo "Intel compiler"
    module load CMake/3.4.1-intel-2015b
}

clean_compiled(){
    echo "Remove $COMPILE_PATH"
    test -e "$COMPILE_PATH" && rm -rf "$COMPILE_PATH"
}

build_routing(){
    echo "----- Start build -----"

    export ROUTING_PROFILER="$PROFILER"
    export ROUTING_BUILD_TYPE="$BUILD_TYPE"
    
    cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -B$COMPILE_PATH -H$ROUTING_MAIN_PATH > /dev/null
    cd $COMPILE_PATH
    make -j 24 > /dev/null
}

run_analyse(){
    RUN_BINARY_PATH=$COMPILE_PATH$BINARY_NAME
    cd $COMPILE_PATH
    chmod +x $RUN_BINARY_PATH

    echo "---- Run binary $RUN_BINARY_PATH ------"
    echo "add params $ADD_RUN_PARAMS"
   
    if [ $ROUTING_PROFILER = "gprof" ]; then
	output_file=$RESULT_PATH$PROFILER/$BUILD_TYPE-$COMPILER-$ADD_RUN_PARAMS$(date '+%d%m%y-%H%M%S')
	
        $RUN_BINARY_PATH -h $HDF_PATH -i $INPUT_ID $ADD_RUN_PARAMS  >> "$output_file".log	
	
	gprof $RUN_BINARY_PATH gmon.out > "$output_file".out
    elif [ $ROUTING_PROFILER = "valgrind" ]; then
        echo "---- finish ----"
    	valgrind $RUN_BINARY_PATH -h $HDF_PATH -i $INPUT_ID
	fi
}

while getopts ":c:b:a:drp:i:" opt; do
  case $opt in
    c)
      echo "compiler, Parameter: $OPTARG" >&2
      COMPILER=$OPTARG
      ;;
    b)
      echo "build type, Parameter: $OPTARG" >&2
      BUILD_TYPE=$OPTARG
      ;;
    a)
      echo "alternatives, Parameter: $OPTARG" >&2
      ADD_RUN_PARAMS="$ADD_RUN_PARAMS -a $OPTARG"
      ;;
    d)
      echo "distances"
      ADD_RUN_PARAMS="$ADD_RUN_PARAMS -d"
     ;;
    r)
      echo "routing"
      ADD_RUN_PARAMS="$ADD_RUN_PARAMS -r"
     ;;
    p)
      echo "profiler, Parameter: $OPTARG" >&2
      PROFILER=$OPTARG
      ;;
    i)
      echo "input id, Parameter: $OPTARG" >&2
      INPUT_ID=$OPTARG
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done


if [ $COMPILER = "gcc" ]; then
    set_gcc_compile
elif [ $COMPILER = "intel" ]; then
    set_intel_compiler
else
    echo "Invalid compiler $COMPILER"
    exit 1
fi

#Je potreba i pro gcc??
module load HDF5/1.8.16-intel-2015b

if [ $BUILD_TYPE = "Debug" ]; then
    COMPILE_PATH=$ROUTING_MAIN_PATH$DEBUG_BUILD_PATH
    clean_compiled

elif [ $BUILD_TYPE = "Release" ]; then
    COMPILE_PATH=$ROUTING_MAIN_PATH$RELEASE_BUILD_PATH
    clean_compiled
else
    echo "Invalid build type $BUILD_TYPE"
    exit 1
fi

build_routing

run_analyse

# intel kompilator, gcc
# promazat build/debug - clean
# release a debug verze
# kompilator vyber
# typ kompilace - gprof, valgrind
# typ testu
# spusteni testu
# zpracovani vysledku


#100, 1000, 10000 id
# -r, -d, -a 3
#comp, 
# debug release

