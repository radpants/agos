cmake_minimum_required(VERSION 3.17)
project(agos C)

set(CMAKE_C_STANDARD 99)

# raylib, don't build games or examples
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/raylib)

set(CMAKE_C_FLAGS -Wall)

include_directories(vendor/raygui/src/)

add_executable(agos main.c ui.c ui.h world.c world.h hex.c hex.h pch.h)
target_precompile_headers(agos PRIVATE pch.h)
target_link_libraries(agos PRIVATE raylib)