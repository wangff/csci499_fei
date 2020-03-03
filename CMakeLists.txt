cmake_minimum_required(VERSION 3.15)

project(Faas)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(proto)

# all source code
add_subdirectory(src/KeyValueStore)
add_subdirectory(src/Func)
add_subdirectory(src/Warble)
add_subdirectory(src/CommandLine)
# all tests
#add_subdirectory(test/KeyValueStore)
#add_subdirectory(test/Func)
add_subdirectory(test)