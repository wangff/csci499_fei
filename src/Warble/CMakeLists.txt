cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

project(Warble)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
add_library(warble STATIC warble_service.h)