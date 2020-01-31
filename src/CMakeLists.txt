cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(KVStore)

find_package(gflags REQUIRED)
find_package(glog 0.4.0 REQUIRED)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# This branch assumes that gRPC and all its dependencies are already installed
# on this system, so they can be located by find_package().

# Find Protobuf installation
# Looks for protobuf-config.cmake file installed by Protobuf's cmake installation.
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP_UNSECURE gRPC::grpc++_unsecure)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

# Proto file
get_filename_component(kvstore_proto "../proto/KeyValueStore.proto" ABSOLUTE)
get_filename_component(kvstore_proto_path "${kvstore_proto}" PATH)

# Generated sources
set(kvstore_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/KeyValueStore.pb.cc")
set(kvstore_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/KeyValueStore.pb.h")
set(kvstore_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/KeyValueStore.grpc.pb.cc")
set(kvstore_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/KeyValueStore.grpc.pb.h")
add_custom_command(
        OUTPUT "${kvstore_proto_srcs}" "${kvstore_proto_hdrs}" "${kvstore_grpc_srcs}" "${kvstore_grpc_hdrs}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${kvstore_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${kvstore_proto}"
        DEPENDS "${kvstore_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}/")

set(kvstore_srcs "KeyValueStore/threadsafe_map.cc")

add_executable(KVStore_server KeyValueStore/keyvaluestore_server.cc ${kvstore_proto_srcs} ${kvstore_grpc_srcs} ${kvstore_srcs})
add_executable(KVStore_client Func/keyvaluestore_client.cc ${kvstore_proto_srcs} ${kvstore_grpc_srcs} ${kvstore_srcs})


target_link_libraries(KVStore_server ${_GRPC_GRPCPP_UNSECURE} ${_PROTOBUF_LIBPROTOBUF})
target_link_libraries(KVStore_server glog::glog)
target_link_libraries(KVStore_server gflags)
target_link_libraries(KVStore_server ${GTEST_LIBRARIES} pthread)


target_link_libraries(KVStore_client ${_GRPC_GRPCPP_UNSECURE} ${_PROTOBUF_LIBPROTOBUF})
target_link_libraries(KVStore_client glog::glog)
target_link_libraries(KVStore_client gflags)
target_link_libraries(KVStore_client ${GTEST_LIBRARIES} pthread)