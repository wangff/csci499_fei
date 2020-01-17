cmake_minimum_required(VERSION 3.15)
project(MergeSort)

set(CMAKE_CXX_STANDARD 11)

add_executable(MergeSort MergeSort.h MergeSort.cpp test.cpp)
