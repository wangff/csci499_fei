cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(Func)

find_package(gflags REQUIRED)

find_package(glog 0.4.0 REQUIRED)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

find_package(gRPC CONFIG REQUIRED)
set(_GRPC_GRPCPP_UNSECURE gRPC::grpc++_unsecure)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

# Use top level of cmake build.
include_directories(
    ${CMAKE_BINARY_DIR}
)

# Func Service
add_executable(func_server func_service.cc func_platform.cc func_platform.h storage_abstraction.h keyvaluestore_client.cc keyvaluestore_client.h ../Warble/warble_service_abstraction.h ../Warble/warble_service.cc ../Warble/warble_service.h ../Warble/profile.h)

# KeyValue Client

foreach(_target func_server)
  target_link_libraries(${_target} key_value_store_pb)
  target_link_libraries(${_target} func_pb)
  target_link_libraries(${_target} warble_pb)

  target_link_libraries(${_target} ${_GRPC_GRPCPP_UNSECURE} ${_PROTOBUF_LIBPROTOBUF})
  target_link_libraries(${_target} glog::glog)
  target_link_libraries(${_target} gflags)
  target_link_libraries(${_target} ${GTEST_LIBRARIES} pthread)
endforeach()



