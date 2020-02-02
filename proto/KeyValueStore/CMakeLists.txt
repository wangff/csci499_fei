cmake_minimum_required(VERSION 3.5.1)

project(KeyValueStoreProto)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

find_package(Threads REQUIRED)

# This branch assumes that gRPC and all its dependencies are already installed
# on this system, so they can be located by find_package().

# Find Protobuf installation
# Looks for protobuf-config.cmake file installed by Protobuf's cmake installation.
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)


# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP_UNSECURE gRPC::grpc++_unsecure)

if (CMAKE_CROSSCOMPILING)
  find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else ()
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif ()

# Proto file
get_filename_component(keyvaluestore_proto "KeyValueStore.proto" ABSOLUTE)
get_filename_component(keyvaluestore_proto_path "${keyvaluestore_proto}" PATH)

# Generated sources
set(keyvaluestore_proto_srcs "${CMAKE_BINARY_DIR}/KeyValueStore.pb.cc")
set(keyvaluestore_proto_hdrs "${CMAKE_BINARY_DIR}/KeyValueStore.pb.h")
set(keyvaluestore_grpc_srcs "${CMAKE_BINARY_DIR}/KeyValueStore.grpc.pb.cc")
set(keyvaluestore_grpc_hdrs "${CMAKE_BINARY_DIR}/KeyValueStore.grpc.pb.h")

add_custom_command(
    OUTPUT "${keyvaluestore_proto_srcs}" "${keyvaluestore_proto_hdrs}" "${keyvaluestore_grpc_srcs}" "${keyvaluestore_grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_BINARY_DIR}"
    --cpp_out "${CMAKE_BINARY_DIR}"
    -I "${keyvaluestore_proto_path}"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    "${keyvaluestore_proto}"
    DEPENDS "${keyvaluestore_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_BINARY_DIR}")

add_library(key_value_store_pb STATIC ${keyvaluestore_proto_srcs} ${keyvaluestore_grpc_srcs})
target_include_directories(key_value_store_pb PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(key_value_store_pb ${_GRPC_GRPCPP_UNSECURE} ${_PROTOBUF_LIBPROTOBUF})