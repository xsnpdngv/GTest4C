# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(CppHashExample C CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable( main
    main.c
    hash.cpp
)
