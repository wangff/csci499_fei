cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(Warble)

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
#find_package(gRPC CONFIG REQUIRED)
#set(_GRPC_GRPCPP_UNSECURE gRPC::grpc++_unsecure)
#set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

include_directories("${CMAKE_BINARY_DIR}")

#add_executable(Warble_service warble_service.cc  warble_service.h ../Func/storage_abstraction.h)

#target_link_libraries(Warble_service key_value_store_pb)
#target_link_libraries(Warble_service ${_GRPC_GRPCPP_UNSECURE} ${_PROTOBUF_LIBPROTOBUF})
#target_link_libraries(Warble_service glog::glog)
#target_link_libraries(Warble_service gflags)
#target_link_libraries(Warble_service ${GTEST_LIBRARIES} pthread)

