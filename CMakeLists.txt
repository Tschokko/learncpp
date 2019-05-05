# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.0)

# Project's name
project(learncpp)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Set C++11 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-msse4 -O2 -Wall -Wextra")

# Include files
include_directories("${PROJECT_SOURCE_DIR}/include")
# include_directories(thirdparty/nlohmann/json/include)

# Source files
file(GLOB SOURCES "src/*.cpp")

#
# Third party sources
#

# Abseil C++
set(ABSL_RUN_TESTS OFF)
set(ABSL_USE_GOOGLETEST_HEAD OFF)
set(BUILD_TESTING OFF)
add_subdirectory(thirdparty/abseil-cpp)

# JSON
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(thirdparty/nlohmann/json)

# Websocketpp
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
include_directories(thirdparty/websocketpp)
include_directories(thirdparty/asio-1.12.2/include)
add_subdirectory(thirdparty/websocketpp)

# spdlog
include_directories(thirdparty/spdlog/include)
add_subdirectory(thirdparty/spdlog)

# Our target
add_executable(learncpp ${SOURCES})
# 
target_link_libraries(learncpp Threads::Threads absl::base absl::memory nlohmann_json::nlohmann_json)
