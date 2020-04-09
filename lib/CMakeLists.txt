
cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE NET_LIB_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/net/*.cpp")
add_library(net STATIC "${NET_LIB_SRCS}")
target_include_directories(net PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_target_properties(net PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)
