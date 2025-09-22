# CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(GTest4C C CXX)

set(CMAKE_CXX_STANDARD 17) # GTest requires at least C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors -fpermissive -Wno-write-strings -Wno-format-truncation")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wno-format-truncation")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(COV "Enable coverage reporting" OFF)
if(COV)
    message(STATUS "Building with coverage flags")
    set(COVERAGE_FLAGS "--coverage -O0 -g")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COVERAGE_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
  # URL file://${CMAKE_CURRENT_SOURCE_DIR}/googletest-1.17.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)
include(CTest)


add_library( logger SHARED
    lib/logger/logger.c
)


include_directories(
    {GTEST_INCLUDE_DIRS}
    lib/logger
    src
    mock
)


add_executable( greeter_test
    tests/greeter_test.cpp
    src/greeter.c
)
target_link_libraries( greeter_test ${GTEST_LIBRARIES} gmock gmock_main pthread logger )
gtest_discover_tests( greeter_test )


add_executable( greeter_test_fixture
    tests/greeter_test_fixture.cpp
    src/greeter.c
)
target_link_libraries( greeter_test_fixture ${GTEST_LIBRARIES} gmock gmock_main pthread logger )
gtest_discover_tests( greeter_test_fixture )


add_executable( greeter_param_test
    tests/greeter_param_test.cpp
    src/greeter.c
)
target_link_libraries( greeter_param_test ${GTEST_LIBRARIES} gmock gmock_main pthread logger )
gtest_discover_tests( greeter_param_test )


add_executable( greeter_death_test
    tests/greeter_death_test.cpp
    src/greeter.c
)
target_link_libraries( greeter_death_test ${GTEST_LIBRARIES} gmock gmock_main pthread logger )
gtest_discover_tests( greeter_death_test )


add_executable( greeter_mock_test
    tests/greeter_mock_test.cpp
    src/greeter.c
    mock/logger_mock.cpp
)
target_link_libraries( greeter_mock_test ${GTEST_LIBRARIES} gmock gmock_main pthread )
gtest_discover_tests( greeter_mock_test )
