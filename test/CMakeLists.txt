cmake_minimum_required(VERSION 3.15)

project(Faas_tests)

set(CMAKE_CXX_STANDARD 17)

find_package(gflags REQUIRED)
find_package(glog 0.4.0 REQUIRED)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(BINARY Faas_tests)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/Func
    ${CMAKE_SOURCE_DIR}/src/KeyValueStore
    ${CMAKE_SOURCE_DIR}/test/Func
    ${CMAKE_SOURCE_DIR}/test/KeyValueStore
)

file(GLOB_RECURSE FUNC_TEST_SOURCES LIST_DIRECTORIES false Func *.h *.cc)
file(GLOB_RECURSE KEYVALUESTORE_TEST_SOURCES LIST_DIRECTORIES false KeyValueStore *.h *.cc)

add_executable(${BINARY} main.cc ${FUNC_TEST_SOURCES} ${KEYVALUESTORE_TEST_SOURCES} ${CMAKE_SOURCE_DIR}/src/KeyValueStore/threadsafe_map.cc)

# link generated pb library
target_link_libraries(${BINARY} key_value_store_pb)
# other libraries we use
target_link_libraries(${BINARY} glog::glog)
target_link_libraries(${BINARY} gflags)
target_link_libraries(${BINARY} ${GTEST_LIBRARIES} pthread)