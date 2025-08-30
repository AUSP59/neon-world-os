
cmake_minimum_required(VERSION 3.22)
project(single_include_example CXX)
include_directories(${CMAKE_SOURCE_DIR}/single_include)
add_executable(single_include main.cpp)
