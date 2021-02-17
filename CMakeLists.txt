cmake_minimum_required(VERSION 3.17)
project(agos C)

set(CMAKE_C_STANDARD 99)

# raylib, don't build games or examples
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/raylib)

set(CMAKE_C_FLAGS -Wall)

if(WIN32)
    set(CMAKE_C_FLAGS "/EHsc /W4")
    # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /O2 /Oi /Ot")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd5045") # spectre mitigation, don't care
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4239") # non standard extension
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4996") # strcpy is FINE JEEZ
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4204") # thing_t thing = { 1, 2, "popsicle" } (LET ME DO THIS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4255") # allow void func() instead of void func(void)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SUBSYSTEM:CONSOLE")
endif()


include_directories(vendor/raygui/src/)
include_directories(vendor/)

add_executable(agos main.c ui.c ui.h world.c world.h hex.c hex.h pch.h array.c array.h)
target_precompile_headers(agos PRIVATE pch.h)
target_link_libraries(agos PRIVATE raylib)

file(COPY data DESTINATION .)
