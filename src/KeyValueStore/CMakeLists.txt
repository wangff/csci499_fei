cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

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

add_executable(KVStore_server keyvaluestore_server.cc  keyvaluestore_server.h threadsafe_map.cc threadsafe_map.h)

target_link_libraries(KVStore_server key_value_store_pb)
target_link_libraries(KVStore_server ${_GRPC_GRPCPP_UNSECURE} ${_PROTOBUF_LIBPROTOBUF})
target_link_libraries(KVStore_server glog::glog)
target_link_libraries(KVStore_server gflags)
target_link_libraries(KVStore_server ${GTEST_LIBRARIES} pthread)

