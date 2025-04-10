cmake_minimum_required(VERSION 3.10)
project("Approximate Distance Oracles" LANGUAGES CXX VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Source files
set(SOURCES
    src/Graph.cpp
    src/ApproxDistanceOracles.cpp
    src/ado.cpp
    src/Benchmark.cpp
)

set(SOURCES2
    src/Graph.cpp
    src/ApproxDistanceOracles.cpp
    src/distance.cpp
    src/Benchmark.cpp
)

# Define the executable
add_executable(approx_dist_oracle ${SOURCES})


target_include_directories(approx_dist_oracle PRIVATE ${CMAKE_SOURCE_DIR}/include/ ${CMAKE_SOURCE_DIR}/src/)
target_compile_options(approx_dist_oracle PRIVATE -Wall -Wextra)

# Add the second executable
add_executable(true_distance ${SOURCES2})

target_include_directories(true_distance PRIVATE ${CMAKE_SOURCE_DIR}/include/ ${CMAKE_SOURCE_DIR}/src/)
target_compile_options(true_distance PRIVATE -Wall -Wextra)