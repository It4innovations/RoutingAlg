cmake_minimum_required(VERSION 3.22.1)
project(Routing)

#
# Compiler flags
#
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -Wall -Wno-invalid-offsetof")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -xHost")
endif ()

#if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
#endif ()


#
# Options
#

# Intel MKL
option(USE_MKL "Use Intel MKL for PTDR" OFF)
if(USE_MKL)
    add_definitions(-DUSE_MKL)
    set(MKL_LIBS mkl_intel_lp64 mkl_sequential mkl_core pthread m dl)
endif()

#
# Dependencies
#

#
# OpenMP
#
find_package(OpenMP COMPONENTS "CXX" REQUIRED)
if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()


# Find non-cmake built HDF5
if (WIN32 OR CYGWIN)
    set(ENV{HDF5_ROOT} "/Program Files/HDF_Group/HDF5/1.10.0")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    #set(ENV{HDF5_ROOT} "/opt/hdf5") # Non-standard location, change this when building on Mac OS X
    set(MKL_ROOT "/opt/intel/mkl")
    include_directories(${MKL_ROOT}/include) # Non-standard location, change this when building on Mac OS X
    link_directories(${MKL_ROOT}/lib)
endif()

find_package(HDF5 COMPONENTS "CXX" REQUIRED)
if(HDF5_FOUND)
    include_directories(${HDF5_INCLUDE_DIRS})
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

#
# SQLite
#

find_path(SQLITE3_INCLUDE_DIR NAMES sqlite3.h HINTS $ENV{EBROOTSQLITE}/include)
find_library(SQLITE3_LIBRARY NAMES sqlite3 HINTS $ENV{EBROOTSQLITE}/lib)

include_directories(${SQLITE3_INCLUDE_DIR})

message(STATUS "SQLite ${SQLITE3_INCLUDE_DIR}, ${SQLITE3_LIBRARY}")

#
# Build info
#
#execute_process(COMMAND git rev-parse HEAD
#        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#        OUTPUT_VARIABLE GIT_REVISION
#        ERROR_VARIABLE GIT_ERROR)
#string(REGEX REPLACE "\n$" "" GIT_REVISION "${GIT_REVISION}")
#if(GIT_ERROR)
#    message(FATAL_ERROR "Git command failed: ${GIT_ERROR}")
#endif()

configure_file(
        ${PROJECT_SOURCE_DIR}/Routing/Algorithms/Common/Build.h.in
        ${PROJECT_SOURCE_DIR}/Routing/Algorithms/Common/Build.h
)

#
# Doxygen
#

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" OFF)
if(BUILD_DOC)
    # check if Doxygen is installed
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        # note the option ALL which allows to build the docs together with the application
        add_custom_target( doc_doxygen ALL
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM )
    else (DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif(BUILD_DOC)

#
# Google tests
#
enable_testing()
# Download and unpack googletest at configure time
configure_file(${PROJECT_SOURCE_DIR}/Tests/CMakeLists.txt.in ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

#
# INIH
#
add_library(inih SHARED
        include/inih/INIReader.cpp
        include/inih/ini.cpp)

# Print configuration report
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})
message(STATUS "Use Intel MKL: " ${USE_MKL})
message(STATUS "Compiler flags:" ${CMAKE_CXX_COMPILE_FLAGS})
message(STATUS "Compiler cxx debug flags:" ${CMAKE_CXX_FLAGS_DEBUG})
message(STATUS "Compiler cxx release flags:" ${CMAKE_CXX_FLAGS_RELEASE})
message(STATUS "Compiler cxx min size flags:" ${CMAKE_CXX_FLAGS_MINSIZEREL})
message(STATUS "Compiler cxx flags:" ${CMAKE_CXX_FLAGS})

#
# Source files and targets
#

set(SOURCE_FILES
        api/routing.cpp
        Routing/Data/Indexes/GraphMemory.cpp
        Routing/Data/Indexes/GraphPartMemory.cpp
        Routing/Data/Structures/Geo/GpsRectangle.cpp
        Routing/Data/Utility/DistanceCalculator.cpp
        Routing/Algorithms/Common/AlgorithmHelper.cpp
        Routing/Algorithms/Common/Result.cpp
        Routing/Algorithms/OneToOne/Astar/BidirectionalAstar.cpp
        Routing/Algorithms/OneToOne/Astar/Astar.cpp
        Routing/Algorithms/OneToOne/Dijkstra/Dijkstra.cpp
        Routing/Algorithms/OneToOne/Dijkstra/BidirectionalDijkstra.cpp
        Routing/Algorithms/Alternatives/RouteSolution.cpp
        Routing/Algorithms/Common/DistanceInfo.cpp
        Routing/Algorithms/Common/DistanceResult.cpp
        Routing/Hdf/DataIndex.cpp
        Routing/Hdf/DataIndexPart.cpp
        Routing/Hdf/Types/RoutingIndexInfoCompType.cpp
        Routing/Tests/IOUtils.cpp
        Routing/Algorithms/Common/SegmentBase.cpp
        Routing/Algorithms/Common/Segment.cpp
        Routing/Algorithms/Base/StaticRoutingAlgorithm.cpp
        Routing/Algorithms/Base/GoalDirectedAlgorithm.cpp
        Routing/Algorithms/Probability/Data/CSVReader.cpp
        Routing/Algorithms/Probability/Data/ProbabilityResult.cpp
        Routing/Algorithms/Probability/Base/ProbabilityAlgorithm.cpp
        Routing/Algorithms/Probability/Algorithms/MonteCarloProbability.cpp
        Routing/Algorithms/Probability/Data/ProbabilityProfile.cpp
        Routing/Algorithms/Probability/Helpers/Utility.cpp
        Routing/Tests/IdPair.cpp
        Routing/Algorithms/Common/SegmentBase.cpp
        Routing/Algorithms/Common/Segment.cpp
        Routing/Algorithms/Common/VisitedNode.cpp
        Routing/Algorithms/Alternatives/AlternativesAlgorithm.cpp
        Routing/Algorithms/Alternatives/AlternativesPlateauAlgorithm.cpp
        Routing/Data/Probability/ProfileStorageBase.cpp
        Routing/Data/Probability/ProfileStorageHDF5.cpp
        Routing/Pipeline/RoutingPipeline.cpp
        Routing/Pipeline/RoutingPipeline.h
        Routing/Pipeline/Workers/RoutingPipelineWorkerBase.cpp
        Routing/Pipeline/Workers/RoutingPipelineWorkerBase.h
        Routing/Pipeline/PipelineConfig.h
        Routing/Pipeline/RoutingRequest.h
        Routing/Pipeline/Workers/OneToOneWorker.cpp
        Routing/Pipeline/Workers/OneToOneWorker.h
        Routing/Pipeline/PipelineConfig.cpp
        Routing/Pipeline/Workers/PTDRWorker.cpp
        Routing/Pipeline/Workers/PTDRWorker.h
        Routing/Pipeline/RoutingRequest.cpp
        Routing/Data/Utility/FilesystemUtil.cpp
        Routing/Data/Probability/ProfileStorageHDF5.cpp
        Routing/Algorithms/TDAlgorithms/TDBase.h
        Routing/Algorithms/TDAlgorithms/TDDijkstra.h
        Routing/Algorithms/TDAlgorithms/TDDijkstra.cpp
        Routing/Algorithms/TDAlgorithms/TDDAlternativesUtility.h
        Routing/Algorithms/TDAlgorithms/TDDAlternativesUtility.cpp
        Routing/Algorithms/TDAlgorithms/TDUtility.h
        Routing/Algorithms/TDAlgorithms/TDUtility.cpp
        Routing/Algorithms/GraphFilter/GraphFilterBase.h
        Routing/Algorithms/GraphFilter/GraphFilterGeometry.cpp
        Routing/Algorithms/GraphFilter/GraphFilterGeometry.h
        Routing/Algorithms/GraphFilter/GraphFilterHierarchy.h
        Routing/Algorithms/GraphFilter/GraphFilterGeometryStartLocation.h
        Routing/Data/Utility/GPSDistanceCalculation.h
        Routing/Pipeline/Workers/AlternativesPlateauWorker.cpp
        Routing/Pipeline/Workers/AlternativesPlateauWorker.h
        Routing/Pipeline/Workers/TDDijkstraWorker.cpp
        Routing/Pipeline/Workers/TDDijkstraWorker.h
        Routing/Data/Utility/GPSDistanceCalculation.h
        Routing/Data/Results/PipelineResultStorageBase.cpp
        Routing/Data/Results/PipelineResultStorageBase.h
        Routing/Data/Results/PipelineResultStorageSQLite.cpp
        Routing/Data/Results/PipelineResultStorageSQLite.h
        Routing/Algorithms/Alternatives/AlternativesPlateauTDPlugin.cpp
        Routing/Algorithms/Alternatives/AlternativesPlateauTDPlugin.h
        Routing/Data/Results/PipelineResultStorageCSV.cpp
        Routing/Data/Results/PipelineResultStorageCSV.h
        Routing/Algorithms/Reordering/ReorderingBase.cpp
        Routing/Algorithms/Reordering/ReorderingBase.h
        Routing/Algorithms/Reordering/SessionReordering.cpp
        Routing/Algorithms/Reordering/SessionReordering.h
        Routing/Pipeline/Workers/ReorderingWorker.cpp
        Routing/Pipeline/Workers/ReorderingWorker.h
        Routing/Algorithms/Common/FilterSettings.h
        Routing/Algorithms/OneToMany/OneToManyDijkstra.cpp
        Routing/Algorithms/OneToMany/OneToManyDijkstra.h
        Routing/Data/Collections/BinaryHeap.h
        Routing/Algorithms/OneToMany/DistanceMatrix.cpp
        Routing/Algorithms/OneToMany/DistanceMatrix.h
        FiltersSettings/main-diff-evo.cpp FiltersSettings/DifferentialEvolution.cpp FiltersSettings/DifferentialEvolution.h FiltersSettings/DiffEvoIndividual.cpp FiltersSettings/DiffEvoIndividual.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Build routing library
add_library(routing SHARED ${SOURCE_FILES})
target_link_libraries(routing ${HDF5_LIBRARIES} ${HDF5_CXX_LIBRARIES} ${MPI_CXX_LIBRARIES} ${MKL_LIBS} ${SQLITE3_LIBRARY} inih)

# Add routing cli endpoint
add_executable(routing-cli main-routing.cpp)
target_link_libraries(routing-cli routing)

# Add routing alt endpoint
add_executable(routing-alt main-alt.cpp)
target_link_libraries(routing-alt routing)

# Define paths to test data
set(HDF_PROFILE_PATH "${PROJECT_SOURCE_DIR}/data/speed_profiles/speed_profiles_alias.h5")
set(HDF_INDEX_PATH "${PROJECT_SOURCE_DIR}/data/routing_indexes/CZE-1528295206-proc-20180724134457.hdf")
set(ID_QUERY_SET "${PROJECT_SOURCE_DIR}/data/routes/05_random_cities.csv")
set(ID_QUERY_SET_TIME_TEST "${PROJECT_SOURCE_DIR}/data/routes/04_cities_cz_time.csv")
set(QUERY_SET_RESULTS_DIR "${PROJECT_SOURCE_DIR}/data/routes/05_random_cities_results")
set(PIPELINE_CONFIG "${CMAKE_CURRENT_BINARY_DIR}/data/pipeline.conf")

# Testing target for GTest execution
configure_file(${PROJECT_SOURCE_DIR}/Tests/DataPaths.h.in ${PROJECT_SOURCE_DIR}/Tests/DataPaths.h)
configure_file(${PROJECT_SOURCE_DIR}/data/pipeline.conf.in ${PIPELINE_CONFIG})
add_executable(routing-test ${PROJECT_SOURCE_DIR}/Tests/main.cpp)
target_link_libraries(routing-test routing gtest_main)
add_test(NAME StaticRoutingCPP_Tests COMMAND routing-test)

# Differential Evolution for filters settings
add_executable(filters-diff-evo ${PROJECT_SOURCE_DIR}/FiltersSettings/main-diff-evo.cpp)
target_link_libraries( filters-diff-evo routing ${HDF5_CXX_LIBRARIES} ${HDF5_LIBRARIES})
set(HDF_INDEX_PATH "${PROJECT_SOURCE_DIR}/data/routing_indexes/CZE-1528295206-proc-20180724134457.hdf")