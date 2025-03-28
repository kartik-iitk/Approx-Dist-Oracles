cmake_minimum_required(VERSION 3.10)
project("Approximate Distance Oracles" LANGUAGES CXX VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(APPLE)
    # Include the header files for using bits/stdc++.h
    include_directories(
        /usr/local/include/
        )
    endif()

# Source files
set(SOURCES
    src/Graph.cpp
    src/ApproxDistanceOracles.cpp
    src/main.cpp
)

# Define the executable
add_executable(approx_dist_oracle ${SOURCES})

target_compile_options(approx_dist_oracle PRIVATE -Wall -Wextra)