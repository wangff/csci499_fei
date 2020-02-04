cmake_minimum_required(VERSION 3.15)
project(KeyValueStore)

find_package(gflags REQUIRED)
find_package(glog 0.4.0 REQUIRED)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 17)

add_executable(KeyValueStore main.cc threadsafe_map_unittest.cc ../../src/KeyValueStore/threadsafe_map.cc)

target_link_libraries(KeyValueStore glog::glog)
target_link_libraries(KeyValueStore gflags)
target_link_libraries(KeyValueStore ${GTEST_LIBRARIES} pthread)