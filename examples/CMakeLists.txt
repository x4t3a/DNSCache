
cmake_minimum_required(VERSION 3.10)

set(EXAMPLE_DNS_CACHE_APP example_dns_cache)
add_executable("${EXAMPLE_DNS_CACHE_APP}" "${CMAKE_CURRENT_SOURCE_DIR}/example_dns_cache.cpp")
target_link_libraries("${EXAMPLE_DNS_CACHE_APP}" net)
target_include_directories("${EXAMPLE_DNS_CACHE_APP}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set_target_properties("${EXAMPLE_DNS_CACHE_APP}" PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)
