cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(BRGBAEMU)

include_directories(
    core/include
)

add_library(brgbacore
    core/src/bus.cpp
    core/src/cpu.cpp    

    core/include/gba_core.h
)
add_executable(brgbatest
    core_test/src/main.cpp
    core_test/src/cpu_test.cpp

    core_test/include/cpu_test.h
)
target_link_libraries(
    brgbatest brgbacore
)
