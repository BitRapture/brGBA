cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(BRGBAEMU)

include_directories(
    core/include
)

add_library(brgbacore
    
    core/include/gba_core.h
)
add_executable(brgbatest
    core_test/src/main.cpp
)
target_link_libraries(
    brgbatest brgbacore
)