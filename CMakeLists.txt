cmake_minimum_required(VERSION 3.17)
project(agos C)

set(CMAKE_C_STANDARD 99)

# raylib, don't build games or examples
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/raylib)

set(CMAKE_C_FLAGS -Wall)

add_executable(agos main.c)
target_link_libraries(agos PRIVATE raylib)