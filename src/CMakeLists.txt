# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(cBuildWithCMake C)

add_library( logger SHARED
    ../lib/logger/logger.c
)

include_directories(
    ../lib/logger
)

add_executable( module_m
    module_m.c
    greeter.c
)
target_link_libraries( module_m
    logger
)
