# Minimum CMake version required.
cmake_minimum_required(VERSION 3.10)

# Project name.
project(DynamicRouteOptimizerCppEngine)

# Set C++ standard to C++17.
set(CMAKE_CXX_STANDARD 17)
# Enforce C++17 standard.
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add include directory for header files.
include_directories(include)

# Create a list of all source files.
set(SOURCES
    main.cpp
    utils/graph.cpp
    utils/graph_io.cpp
    utils/segment_tree.cpp
    algorithms/dijkstra.cpp
    algorithms/astar.cpp
    algorithms/floyd_warshall.cpp
    algorithms/union_find.cpp
)

# Add executable target.
add_executable(dynamic_route_optimizer ${SOURCES})