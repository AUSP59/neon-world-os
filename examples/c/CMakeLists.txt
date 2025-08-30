
cmake_minimum_required(VERSION 3.22)
project(example_c C)
find_package(NEONWorldOS REQUIRED)
add_executable(example_c main.c)
target_link_libraries(example_c PRIVATE neon::neonc)
