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
add_executable(func_service keyvaluestore_client.cc keyvaluestore_client.h)

# KeyValue Client
#add_executable(KVStore_client keyvaluestore_client.cc)

#foreach(_target func_service KVStore_client)
foreach(_target func_service)
  target_link_libraries(${_target} key_value_store_pb)
  target_link_libraries(${_target} ${_GRPC_GRPCPP_UNSECURE} ${_PROTOBUF_LIBPROTOBUF})
  target_link_libraries(${_target} glog::glog)
  target_link_libraries(${_target} gflags)
  target_link_libraries(${_target} ${GTEST_LIBRARIES} pthread)
endforeach()



