cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(Faas_tests)

# for finding GMock.
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package(gflags REQUIRED)
find_package(glog 0.4.0 REQUIRED)
find_package(GTest REQUIRED)
find_package(GMock REQUIRED)

set(BINARY Faas_tests)
include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/Func
    ${CMAKE_SOURCE_DIR}/src/KeyValueStore
    ${CMAKE_SOURCE_DIR}/src/Warble

    ${CMAKE_SOURCE_DIR}/test/Func
    ${CMAKE_SOURCE_DIR}/test/KeyValueStore
    
    ${GMOCK_INCLUDE_DIRS}
)

file(GLOB_RECURSE FUNC_TEST_SOURCES LIST_DIRECTORIES false Func *.h *.cc)
file(GLOB_RECURSE KEYVALUESTORE_TEST_SOURCES LIST_DIRECTORIES false KeyValueStore *.h *.cc)

#file(GLOB_RECURSE FUNC_SOURCES LIST_DIRECTORIES false KeyValueStore *.h *.cc)
#file(GLOB_RECURSE KEYVALUESTORE_SOURCES LIST_DIRECTORIES false KeyValueStore *.h *.cc)

add_executable(${BINARY} main.cc
        ${FUNC_TEST_SOURCES}
        ${CMAKE_SOURCE_DIR}/src/Func/func_platform.cc
        ${CMAKE_SOURCE_DIR}/src/Func/keyvaluestore_client.cc

        ${KEYVALUESTORE_TEST_SOURCES}
        ${CMAKE_SOURCE_DIR}/src/KeyValueStore/threadsafe_map.cc
        ${CMAKE_SOURCE_DIR}/src/Func/storage_abstraction.h
    )

# link generated pb library
target_link_libraries(${BINARY} key_value_store_pb)
target_link_libraries(${BINARY} func_pb)

# other libraries we use
target_link_libraries(${BINARY} glog::glog)
target_link_libraries(${BINARY} gflags)
target_link_libraries(${BINARY} GTest::GTest GTest::Main)

message(STATUS "\"${BINARY}\" will link mock libraries: \"${GMOCK_BOTH_LIBRARIES}\"")

target_link_libraries(${BINARY} ${GMOCK_BOTH_LIBRARIES})