
cmake_minimum_required(VERSION 3.10)

project("DNSCache")

option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_UT "Build unit-tests" OFF)

add_subdirectory(lib)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_UT)
    add_subdirectory(tests)
endif()

