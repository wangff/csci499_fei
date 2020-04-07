cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-std=c++17 -lstdc++fs")

project(KeyValueStore)

# gflags
find_package(gflags REQUIRED)

# glog
find_package(glog 0.4.0 REQUIRED)

# gTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Proto
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

# gRPC
find_package(gRPC CONFIG REQUIRED)
set(_GRPC_GRPCPP_UNSECURE gRPC::grpc++_unsecure)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

include_directories("${CMAKE_BINARY_DIR}")

set(BINARY kvstore_server)

add_executable(${BINARY} keyvaluestore_server.cc  keyvaluestore_server.h threadsafe_map.cc threadsafe_map.h persistence.cc persistence.h persistence_abstraction.h)
target_link_libraries(${BINARY} stdc++fs)

target_link_libraries(${BINARY} key_value_store_pb)
target_link_libraries(${BINARY} ${_GRPC_GRPCPP_UNSECURE} ${_PROTOBUF_LIBPROTOBUF})
target_link_libraries(${BINARY} glog::glog)
target_link_libraries(${BINARY} gflags)
target_link_libraries(${BINARY} ${GTEST_LIBRARIES} pthread)

