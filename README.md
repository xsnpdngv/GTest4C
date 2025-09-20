% GoogleTest for Testing C Code  
% Tamás Dezső  
% Sept 20, 2025  
<!-- pandoc README.md -o GTest_for_C_v1.0.pdf \
    -V papersize:A4 \
    -V documentclass=scrartcl \
    -V geometry:margin=1in \
    -V colorlinks \
    --toc \
    --toc-depth=2 \
    --pdf-engine=xelatex \
    -V mainfont='Roboto Light' \
    -V monofont='Ubuntu Mono' \
    -V fontsize=10pt
-->


# Outline

1. What is GTest
2. What are the advantages of using Gtest in general
3. What are the differences between CUnit and GTest
4. How can C and C++ code work together
5. Google Test primer
6. Google Mock for dummies
7. References


# Introduction

GoogleTest (GTest) is one of the most widely used frameworks for
unit testing in C++ projects. While designed for C++, it can also be
applied to C codebases, enabling developers to take advantage of its
expressive assertions, test fixtures, and integration with build and
CI tools. This article explains how to set up GTest for C projects,
demonstrates practical techniques for testing C functions, and
highlights how mocking can improve testability.


# Scope and Focus

In this document the following topics and areas are covered:

- Writing clear and maintainable tests for C code using GoogleTest.
- Using assertions to express expected behavior.
- Applying fixtures to share setup and teardown logic across multiple tests.
- Creating parameterized tests to cover multiple input combinations efficiently.
- Mocking dependencies to test modules in isolation.
- Using death tests to detect crashes, aborts, and segmentation faults safely.
- Integrating tests into CI/CD pipelines with JSON output for automated reporting.
- Submitting test, coverage and memory-check results into open source dashboard

This article walks step-by-step through practical examples and provides
a ready-to-play project structure.


# Advantages of GTest for C Code

GTest offers a powerful, modern unit testing environment even for C projects:

- __Mature and well-supported__: battle-tested and integrated with CI/CD pipelines.
- __Rich feature set__: assertions, fixtures, parameterized tests, death tests, and mocking.
- __Organized tests__: group tests into suites, run selectively, and share setup/teardown.
- __Maintainable reporting__: human-readable output, JSON/XML for CI, clear failure messages.
- __Cross-platform__: works on Linux, macOS, Windows, and embedded systems.
- __Mocking support__: with GoogleMock (gMock), for testing interactions with dependencies.
- __Dashboard integration__: seamless submission of build, test, coverage, and memory-check
  results to CDash, enabling centralized tracking, history, and comparison across builds and platforms.

By adding a thin C++ layer, all of these strengths can be applied directly to C modules.


# CUnit vs GTest

Feature / Aspect          | CUnit                        | GTest
--------------------------|------------------------------|----------------------------------------------
Main function             | Must be implemented manually | Provided by the framework
Adding tests              | Manual registration required | Automatic registration via macros
Assertions                | Limited, basic               | Rich set of assertions (`EXPECT_*`, `ASSERT_*`)
Output                    | Minimal, often unclear       | Detailed, shows file, line, and message
Mocking                   | Not supported                | Supported via GoogleMock
Test fixtures             | Limited                      | Full support for setup/teardown
Parameterized tests       | Not supported                | Fully supported
CI / Reporting            | Poor integration             | Supports XML/JSON output, CI friendly
Selective test execution  | Difficult                    | Easy via test names, regex, or labels
Scalability               | Hard for large projects      | Designed for large projects
Community / Documentation | Small community              | Large community, active maintenance

: Comparison with CUnit

# C and C++ Interoperability

In projects combining C and C++ the main concern is name mangling. C++
compilers mangle function names to support overloading, while C does
not. To allow C++ code to call C functions, use extern "C" in headers.

    gtest4c/
    └─ extern_C/
       ├── hash.cpp
       ├── hash.h
       ├── main.c
       └── CMakeLists.txt

```cpp
// hash.h
#ifndef HASH_WRAP_H
#define HASH_WRAP_H

#include <stddef.h>

size_t hash_string(const char *str);

#endif // HASH_WRAP_H



// hash.cpp
#include <string>
#include <functional>

extern "C" size_t hash_string(const char *str) { return std::hash<std::string>{}(str); }



// main.c
#include "hash.h"

#include <stdio.h>
#include <stdint.h>

int main(void)
{
    const char *s = "Hello, C and C++!";
    size_t h = hash_string(s);
    printf("Hash of '%s' is %zu\n", s, h);
    return 0;
}
```

```bash
cmake -S . -B build
cmake --build build
build/main
# --> Hash of 'Hello, C and C++!' is 2115457373660723382
```


# C Code Under Test

    gtest4c/
    └─ src/
       ├── greeter.c
       └── greeter.h

```c
// greeter.h
#ifndef GREETER_H
#define GREETER_H

typedef struct greeter_t greeter_t;

greeter_t *greeterCreate(const char *greeting);
const char *greeterGreet(greeter_t *self, const char *name);
void greeterDestroy(greeter_t **self);

#endif



// greeter.c
#include "greeter.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct greeter_t
{
    char *greeting; // "Hello", "Hola", "Bonjour", "Ciao", "Üdv", etc
    char buffer[100]; // output goes here
};

greeter_t *greeterCreate(const char *greeting)
{
    if( ! greeting) { return NULL; }
    greeter_t *self = malloc(sizeof(greeter_t));
    self->greeting = strdup(greeting);
    return self;
}

const char *greeterGreet(greeter_t *self, const char *name)
{
    if( ! self) { return NULL; }
    snprintf(self->buffer, sizeof(self->buffer), "%s, %s!", self->greeting, name ?: "World");
    loggerWriteLog(self->buffer);
    return self->buffer;
}

void greeterDestroy(greeter_t **self)
{
    assert(self);
    if( ! *self) { return; }
    free((*self)->greeting);
    free((*self));
    *self = NULL;
}
```


# Asserts: Expressing Expectations Clearly

Assertions are the building blocks of unit testing. They define the
expected behavior of the code and provide meaningful error messages
when the behavior deviates. GTest offers a rich set of assertions that
go far beyond simple equality checks.

GTest distinguishes between:

- `EXPECT_*`: non-fatal assertions: the test continues after a failure.
- `ASSERT_*`: fatal assertions: the test aborts immediately on failure.

This distinction allows precise control over test flow and error reporting.

## Example

```c++
```


For the complete rerefence, see
[Assertions](https://google.github.io/googletest/reference/assertions.html)
in [[GTest Guide][]].

# Fixtures: Sharing Setup and Teardown

When testing C modules that need initialization (e.g., buffers, file
handles, or global state), writing setup and cleanup logic in every test
is error-prone. Fixtures let you centralize preparation and cleanup so
every test starts with a known state.

## Example

```c++
...
```


# Parameterized Testing: Avoiding Repetition

C functions are often tested with a wide variety of inputs. Writing a
separate test for each case can lead to duplicated code. Parameterized
tests allow you to define a single test logic and run it against
multiple input sets automatically.

## Example

```c++
...
```

For the complete reference, see
[`INSTANTIATE_TEST_SUITE_P`](https://google.github.io/googletest/reference/testing.html#INSTANTIATE_TEST_SUITE_P)
in [[GTest Guide][]].

# Mocking: Isolating Dependencies

In C projects, functions often depend on external resources (files,
sockets, hardware APIs). To test reliably, we want to replace real
dependencies with controlled test doubles. While C doesn’t have built-in
mocking, GTest (via GoogleMock) provides a way to mock functions called
from C modules by declaring them in C++.

The Nice, the Strict, and the Naggy

Image how would mocking work with CUnit:
- Each different behavior needs a distinct implementation
- Each implementation needs a distinct build

How are mocks evaluated?

GMock is your friend if any of the following problems is bothering you:

- Your tests are slow as they depend on expensive resources (e.g. a database).
- Your tests are brittle as some resources they use are unreliable (e.g. the network).
- You want to test how your code handles failures but it’s not easy to cause them.
- You need to make sure that your module interacts with other modules in the right way.
- You want to “mock out” the dependencies, except that they don’t have mock implementations yet.


## Example

```c++
...
```


# Death Tests: Verifying Program Termination

In C projects, certain functions may have preconditions that, if
violated, lead to program termination using `abort()`, `assert()`, or
even segmentation faults (`SEGFAULT`s). While normal assertions check
expected outputs, death tests allow you to verify that your code fails
safely and predictably in these critical situations.

GTest runs the code in a separate process, so abnormal termination does
not stop the test runner.

This is particularly useful for:

- Validating assertion failures in C modules.
- Ensuring that critical errors do not go unnoticed.
- Testing safety-critical C code where illegal inputs must terminate the program.

```c++
// Verifies that statement causes the process to terminate with an
// exit status that satisfies predicate, and produces stderr output
// that matches matcher.
EXPECT_EXIT(statement, exit_status_predicate, stderr_matcher)
ASSERT_EXIT(statement, exit_status_predicate, stderr_matcher)

// Returns true if the program exited normally with the given exit
// status code.
::testing::ExitedWithCode(exit_code);

// Returns true if the program was killed by the given signal.
// Not available on Windows.
::testing::KilledBySignal(signal_number);
```

##  Example

```c++
...
```

For the complete reference, see
[Death Assertions](https://google.github.io/googletest/reference/assertions.html#death)
in [[GTest Guide][]].


# Setting Up

To use GTest for C code, the followings are needed:

1.	GoogleTest installed (via package manager or as a submodule in your project).
2.	CMake or Makefile configuration that links C modules into a C++ test executable.
3.	C headers wrapped with extern "C" when included in C++ files, so that function names are not mangled.

_CMakeLists.txt_:

```cmake
cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
  # URL file://${CMAKE_CURRENT_SOURCE_DIR}/googletest-1.17.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
```

For complete reference, see [Quickstart: CMake](https://google.github.io/googletest/quickstart-cmake.html)
in [[GTest Guide][]].

    gtest4c/
    ├─ tests/
    │  ├── greeter_test.cpp
    │  ├── greeter_test_fixture.cpp
    │  ├── greeter_param_test.cpp
    │  ├── greeter_death_test.cpp
    │  └── greeter_mock_test.cpp
    ├─ mock/
    │  ├── logger_mock.cpp
    │  ├── logger_mock.hpp
    │  └── single.hpp
    └- CMakeLists.txt


# CI: Running GTest with JSON Output

Unit tests are most valuable when they run automatically on every
commit, or on a regular basis ensuring regressions are caught early.
Modern CI/CD pipelines (Jenkins, GitHub Actions, GitLab CI, etc.) can
parse test results if GTest produces machine-readable output, such as
JSON or XML.

- Automated verification: Every push triggers tests, preventing broken code from being merged.
- Structured reporting: JSON or XML output allows CI tools to display results, trends, and failed tests clearly.
- Cross-platform support: Works on Linux, macOS, Windows, and even embedded cross-compilation workflows.

## Generating JSON Output

```bash
./mytest --gtest_output=json:results.json
```

```json
{
  "testsuites": [
    {
      "name": "MathUtilsTest",
      "tests": 2,
      "failures": 0,
      "time": 0.002,
      "testsuite": [
        {
          "name": "Add",
          "status": "run",
          "result": "passed",
          "time": 0.001
        },
        {
          "name": "Divide",
          "status": "run",
          "result": "passed",
          "time": 0.001
        }
      ]
    }
  ]
}
```


# CTest

CTest is CMake’s companion test driver. It provides a consistent way to
discover, run, and report unit tests across projects and platforms. When
integrated with frameworks like Google Test, CTest becomes a powerful
tool for local development, CI pipelines, and regression testing.


## Reference

See `man ctest`

Command                     | Description
----------------------------|------------------------------------------------------
`ctest`                     | Run all tests
`ctest -N`                  | List tests without running
`ctest -V`                  | Show test output while running
`ctest --output-on-failure` | Show output only for failed tests
`ctest --rerun-failed`      | Re-run tests that failed last time
`ctest -R <regex>`          | Run tests whose names match regex.
`ctest -R "GreeterMock.*"`  | Run all tests in the `MathTest` suite
`ctest -E <regex>`          | Exclude tests matching regex
`ctest -R ".*" -E "Flaky"`  | Run all tests except those with “Flaky” in the name
`ctest -I <start>,<end>`    | Run tests by index range
`ctest --test-output-json results.json` | Write test results in JSON (CMake ≥ 3.21)
`ctest --show-only=json-v1` | Show test list in JSON format
`ctest -j <N>`              | Run tests in parallel with N jobs
`ctest --timeout <sec>`     | Set a global timeout in seconds
`ctest --repeat-until-fail <N>` | Repeat tests up to N times (find flaky tests)
`ctest --schedule-random`   | Run tests in random order
`ctest --stop-on-failure`   | Stop after the first failing test

: CTest Cheat Sheet

```bash
src_dir=.
build_dir=build

cmake -S ${src_dir} -B ${build_dir} # -DCOV=ON
cmake --build ${build_dir}
ctest --test-dir ${build_dir} -D Experimental
```

## Adding Tests in CMake

To enable CTest in a project, add the following to your CMakeLists.txt:

```cmake
enable_testing()

add_executable(my_tests test_main.cpp)
target_link_libraries(my_tests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(my_tests)
```



# Build

```bash
mkdir build
cd build
cmake ..
make
make test # or ctest
```


# References

- [GTest Guide]: https://google.github.io/googletest/
    [GTest Guide] GoogleTest's User Guide

- [GTest Code]: https://github.com/google/googletest
    [GTest Code] GoogleTest on GitHub
