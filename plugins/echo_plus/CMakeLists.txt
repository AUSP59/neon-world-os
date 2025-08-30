
cmake_minimum_required(VERSION 3.22)
project(neon_echo_plus CXX)
add_library(neon_echo_plus SHARED echo_plus.cpp)
target_include_directories(neon_echo_plus PRIVATE ${CMAKE_SOURCE_DIR}/include)
set_target_properties(neon_echo_plus PROPERTIES PREFIX "" OUTPUT_NAME "neon_echo_plus")
