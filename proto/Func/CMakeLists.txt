cmake_minimum_required(VERSION 3.5.1)

project(FuncProto)

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
get_filename_component(func_proto "Func.proto" ABSOLUTE)
get_filename_component(func_proto_path "${func_proto}" PATH)

# Generated sources
set(func_proto_srcs "${CMAKE_BINARY_DIR}/func.pb.cc")
set(func_proto_hdrs "${CMAKE_BINARY_DIR}/func.pb.h")
set(func_grpc_srcs "${CMAKE_BINARY_DIR}/func.grpc.pb.cc")
set(func_grpc_hdrs "${CMAKE_BINARY_DIR}/func.grpc.pb.h")

add_custom_command(
    OUTPUT "${func_proto_srcs}" "${func_proto_hdrs}" "${func_grpc_srcs}" "${func_grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_BINARY_DIR}"
    --cpp_out "${CMAKE_BINARY_DIR}"
    -I "${func_proto_path}"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    "${func_proto}"
    DEPENDS "${func_proto}")

# Include generated *.pb.h files
# Include generated *.pb.h files

include_directories("${CMAKE_BINARY_DIR}")

add_library(func_pb ${func_proto_srcs} ${func_grpc_srcs})
target_include_directories(func_pb PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(func_pb ${_GRPC_GRPCPP_UNSECURE} ${_PROTOBUF_LIBPROTOBUF})