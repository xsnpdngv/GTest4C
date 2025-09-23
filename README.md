% GoogleTest for Testing C Code  
% Tamás Dezső  
% Sept 23, 2025  
<!-- pandoc README.md -o GTest4C_2025-09-23.pdf \
    -V papersize:A4 \
    -V documentclass=scrartcl \
    -V geometry:margin=1in \
    -V colorlinks \
    --toc --toc-depth=1 \
    --pdf-engine=xelatex \
    -V mainfont='Roboto Light' \
    -V monofont='Ubuntu Mono' \
    -V fontsize=10pt
-->


# Introduction

GoogleTest (GTest) is one of the most widely used frameworks for
unit testing in C++ projects. While designed for C++, it can also be
applied to C codebases, enabling developers to take advantage of its
expressive assertions, test fixtures, and integration with build and
CI tools. This document explains how to set up GTest for C projects,
demonstrates practical techniques for testing C functions, and
highlights how mocking can improve testability.


# Scope and Focus

The following topics and areas are meant to be covered:

- Writing clear and maintainable tests for C code using GoogleTest.
- Using assertions to express expected behavior.
- Applying fixtures to share setup and teardown logic across multiple tests.
- Creating parameterized tests to cover multiple input combinations efficiently.
- Mocking dependencies to test modules in isolation.
- Using death tests to detect crashes, aborts, and segmentation faults safely.
- Integrating tests into CI/CD pipelines with JSON output for automated reporting.
- Submitting test, coverage and memory-check results into open source dashboard.

The following chapters walk step-by-step through practical examples.


# Advantages of GTest

GTest offers a powerful, modern unit testing environment even for C projects:

- __Mature and well-supported__: battle-tested and integrated with CI/CD pipelines
- __Rich feature set__: assertions, fixtures, parameterized tests, death tests
- __Mocking support__: with GoogleMock (gMock), for testing interactions with dependencies
- __Organized tests__: group tests into suites, run selectively, and share setup/teardown
- __Maintainable reporting__: human-readable output, JSON/XML for CI, clear failure messages
- __CMake and CTest integration__: works hand-in-hand with CMake build- and CTest testing framework
- __Cross-platform__: works on Linux, macOS, Windows, and embedded systems
- __Dashboard submission__: seamless submission to CDash, enabling centralized tracking,
  history, and comparison across builds and platforms

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


# CMake and CTest

When building non-trivial C or C++ projects, managing the build process
quickly becomes challenging. Different platforms, compilers, and
development environments have their own requirements. A set of
handwritten Makefiles or ad-hoc scripts often grows messy, hard to
maintain, and difficult to port. This is where CMake comes in.

__CMake__ is a cross-platform build system generator. Instead of directly
compiling your code, it generates native build files (such as Unix
Makefiles, Ninja files, or Visual Studio project files) based on a
high-level configuration written in `CMakeLists.txt`.

With CMake, the necessities of a project can be described (source files,
libraries, include paths, dependencies), and CMake takes care of how to
build it on the target system. This means the same project can be
compiled on Linux, macOS, and Windows without changing your build
description.

__CTest__ is CMake’s companion tool for running tests. If you write unit
or integration tests for your project (for example with GoogleTest,
Catch2, or even plain C test executables), you can register them in your
CMakeLists.txt. CTest then provides a uniform way to discover and
execute tests, report results, and integrate with CI systems and
dashboards (such as CDash).

CMake and CTest let the focus more on code and tests, and give projects
a professional, reproducible, and maintainable build and test setup from
the start.


# C plus C++

In projects combining C and C++ the main concern is name mangling. C++
compilers mangle function names to support overloading, while C does
not. To allow C++ code to call C functions, use __`extern "C"`__ in
headers.

    GTest4C/
    └── externC/
        ├── hash.cpp
        ├── hash.h
        ├── main.c
        └── CMakeLists.txt

```cpp
// hash.h
#ifndef HASH_H_
#define HASH_H_

#include <stddef.h>

size_t hash_string(const char *str);

#endif // HASH_H_
```

```c++
// hash.cpp
extern "C" {
#include "hash.h"
}
#include <string>

size_t hash_string(const char *str) { return std::hash<std::string>{}(str); }
```

```c
// main.c
#include "hash.h"
#include <stdio.h>
#include <stdint.h>

int main(void)
{
    const char *s = "Hello, C plus C++!";
    size_t h = hash_string(s);
    printf("Hash of '%s' is %zu\n", s, h);
    return 0;
}
```

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(CppHashExample C CXX)

add_executable( main
    main.c
    hash.cpp
)
```

## Build and Run

```bash
cd externC
cmake -S . -B build
cmake --build build
build/main
# --> Hash of 'Hello, C plus C++!' is 5909823269864486160
```


# C Code under Test

The following C code is used in the upcoming chapters as the code that
is being tested.

## Code

    GTest4C/
    └── src/
        ├── greeter.c
        └── greeter.h

```c
// greeter.h
#ifndef GREETER_H_
#define GREETER_H_

typedef struct greeter_t greeter_t;

greeter_t *greeterCreate(const char *greeting);
const char *greeterGreet(greeter_t *self, const char *name);
void greeterDestroy(greeter_t **self);

#endif // GREETER_H_
```

```c++
// greeter.c
#include "greeter.h"
#include "logger.h"  // external dependency
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
    loggerWriteLog(self->buffer);  // external dependency
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

## External Dependency

    GTest4C/
    └── lib/
        └── logger/
            ├── logger.c
            └── logger.h

```c
// logger.h
#ifndef LOGGER_H_
#define LOGGER_H_

int loggerWriteLog(const char *message);

#endif // LOGGER_H_
```

## Regular Build

```cmake
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
```

## Build and Run

```bash
cd src
cmake -S . -B build
cmake --build build
build/module_m
# --> [LOG] Hellloooo, Woooorld!
#     Hellloooo, Woooorld!
```


# Setup GTest in CMake

To use GTest for C code, the followings are needed:

1.	GoogleTest installed, present as submodule or fetched on demand.
2.	CMake or Makefile configuration that links C modules into a C++ test executables.
3.	C headers wrapped with extern "C" when included in C++ files, so that function names are not mangled.


```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(GTest4C C CXX)

set(CMAKE_CXX_STANDARD 17) # GTest requires at least C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare( googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)
include(CTest)

include_directories(
    {GTEST_INCLUDE_DIRS}
    lib/logger
    src
    mock
)

# Test executables...
```

For complete reference, see [Quickstart: CMake](https://google.github.io/googletest/quickstart-cmake.html)
in [[GTest Guide][]].


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

    GTest4C/
    ├── src/
    │   ├── greeter.c
    │   └── greeter.h
    ├── tests/
    │   └── greeter_test.cpp
    └── CMakeLists.txt

```cmake
# CMakeLists.txt (excerpt)
add_executable( greeter_test
    tests/greeter_test.cpp
    src/greeter.c
)
target_link_libraries( greeter_test ${GTEST_LIBRARIES} gmock gmock_main pthread logger )
gtest_discover_tests( greeter_test )
```

```c++
// greeter_test.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
extern "C" {
#include "greeter.h"
}

using ::testing::AllOf;
using ::testing::Not;
using ::testing::Lt; // less than
using ::testing::Gt; // greater than
using ::testing::StartsWith;
using ::testing::EndsWith;
using ::testing::HasSubstr;
using ::testing::ContainsRegex;
using ::testing::MatchesRegex;
using ::testing::ResultOf;

/*
TEST(TestSuiteName, TestName) {
  ... test body ...
}
*/

TEST(GreeterTest, DoesntCreateGreeter)
{
    greeter_t *noGreeter = greeterCreate(NULL);
    EXPECT_EQ(noGreeter, nullptr);
}

TEST(GreeterTest, CreatesGreeter)
{
    greeter_t *emptyGreeter = greeterCreate("");
    ASSERT_NE(emptyGreeter, nullptr);
    greeterDestroy(&emptyGreeter);

    greeter_t *formalGreeter = greeterCreate("Good Morning");
    ASSERT_NE(formalGreeter, nullptr);
    greeterDestroy(&formalGreeter);
}

TEST(GreeterTest, DestroysGreeter)
{
    auto friendlyGreeter = greeterCreate("Hi");
    ASSERT_NE(friendlyGreeter, nullptr);

    greeterDestroy(&friendlyGreeter);
    ASSERT_EQ(friendlyGreeter, nullptr);

    greeterDestroy(&friendlyGreeter);
    ASSERT_EQ(friendlyGreeter, nullptr);
}

TEST(GreeterTest, ReturnsNullIfNoSelf)
{
    EXPECT_EQ(greeterGreet(NULL, NULL), nullptr);
    EXPECT_EQ(greeterGreet(NULL, ""), nullptr);
    EXPECT_EQ(greeterGreet(NULL, "asdf"), nullptr);
}

TEST(GreeterTest, GreetsGenerally)
{
    auto g = greeterCreate("Hello");
    EXPECT_STREQ(greeterGreet(g, NULL), "Hello, World!");
    greeterDestroy(&g);
}

TEST(GreeterTest, GreetsPersonally)
{
    auto g = greeterCreate("Good Morning");
    EXPECT_STREQ(greeterGreet(g, "Vietnam"), "Good Morning, Vietnam!");
    EXPECT_STREQ(greeterGreet(g, "Sunshine"), "Good Morning, Sunshine!");
    EXPECT_THAT(greeterGreet(g, "Sunshine"), AllOf(StartsWith("Go"), EndsWith("shine!")));
    greeterDestroy(&g);

    g = greeterCreate("Bonjour");
    const char *str = greeterGreet(g, "Alice");
    EXPECT_STREQ(str, "Bonjour, Alice!");
    EXPECT_THAT(str, HasSubstr("our, Ali"));
    EXPECT_THAT(str, ContainsRegex("on.*ice"));

    str = greeterGreet(g, "Bob");
    EXPECT_STREQ(str, "Bonjour, Bob!");
    EXPECT_THAT(str, MatchesRegex("Bo.*Bo.*"));
    EXPECT_THAT(str, ResultOf(strlen, 13));
    EXPECT_THAT(str, ResultOf([](const char *s){ return std::count(s, s+strlen(s), 'o'); },
                              AllOf(Not(Lt(3)), Not(Gt(3)))));
    greeterDestroy(&g);
}

MATCHER_P2(HasCharCount, ch, charCount,
           "String has " + std::to_string(charCount) + " occurrences of '" + std::string(1, ch) + "'") {
    return charCount == std::count(arg, arg + strlen(arg), ch);
}

TEST(GreeterTest, GreetsPersonallyWithMatcher)
{
    auto g = greeterCreate("Heló");
    auto s = greeterGreet(g, "lila ló");
    EXPECT_THAT(s, HasCharCount('l', 4));
    greeterDestroy(&g);
}
```

## Build and Run Tests

```bash
cmake -S . -B build
cmake --build build
build/greeter_test # ctest --test-dir build -R "GreeterTest\\." --ouput-on-failure
```

## Assert Macros

```c++
EXPECT_TRUE( condition )
EXPECT_FALSE( condition )
EXPECT_EQ( val1, val2 )
EXPECT_NE( val1, val2 )
EXPECT_LT( val1, val2 )  // <
EXPECT_LE( val1, val2 )  // <=
EXPECT_GT( val1, val2 )  // >
EXPECT_GE( val1, val2 )  // >=
EXPECT_STREQ( str1, str2 )
EXPECT_STRNE( str1, str2 )
EXPECT_STRCASEEQ( str1, str2 )
EXPECT_STRCASENE( str1, str2 )
EXPECT_DEATH( statement, matcher )
EXPECT_EXIT( statement, predicate, matcher )
EXPECT_THAT( actual_value, matcher )
```

For the complete reference, see
[Assertions](https://google.github.io/googletest/reference/assertions.html)
in [[GTest Guide][]].

## Matchers

```c++
Eq(value)  // or value
Ge(value)  // >=
Gt(value)  // >
Le(value)  // <=
Lt(value)  // <
Ne(value)  // !=

IsFalse()
IsTrue()
IsNull()
NotNull()

ContainsRegex(string)
EndsWith(suffix)
HasSubstr(string)
IsEmpty()  // ""
MathesRegex(string)
StartsWith(prefix)
StrCaseEq(string)
StrCaseNe(string)
StrEq(string)
StrNe(string)

ResultOf(func, matcher)  // func(argument) matches matcher
AllOf(m1, m2, ..., mn) // argument matches ALL of the matchers m1 to mn.
AnyOf(m1, m2, ..., mn) // argument matches ANY of the matchers m1 to mn.
Conditional(cond, m1, m2) // Matches matcher m1 if cond evaluates to true, else matches m2.

MATCHER(IsEven, "") { return (arg % 2) == 0; } // Defines a matcher IsEven() to match an even number.
```

For the complete reference, see
[Matchers](https://google.github.io/googletest/reference/matchers.html)
in [[GTest Guide][]].


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

    GTest4C/
    ├── src/
    │   ├── greeter.c
    │   └── greeter.h
    ├── tests/
    │   └── greeter_death_test.cpp
    └── CMakeLists.txt

```cmake
# CMakeLists.txt (excerpt)
add_executable( greeter_death_test
    tests/greeter_death_test.cpp
    src/greeter.c
)
target_link_libraries( greeter_death_test ${GTEST_LIBRARIES} gmock gmock_main pthread logger )
gtest_discover_tests( greeter_death_test )
```

```c++
// greeter_death_test.cpp
#include <gtest/gtest.h>
extern "C" {
#include "greeter.h"
}

/*
// Verifies that statement causes the process to terminate with an
// exit status that satisfies predicate, and produces stderr output
// that matches matcher.
EXPECT_EXIT(statement, exit_status_predicate, stderr_matcher)
ASSERT_EXIT(statement, exit_status_predicate, stderr_matcher)

// Returns true if the program exited normally with the given exit status code.
::testing::ExitedWithCode(exit_code);

// Returns true if the program was killed by the given signal.
// Not available on Windows.
::testing::KilledBySignal(signal_number);
*/

using testing::KilledBySignal;
// using testing::ExitedWithCode;

TEST(GreeterDeathTest, AbortsOnDestroyAssert)
{
    EXPECT_EXIT(greeterDestroy(NULL), KilledBySignal(SIGABRT), ".*Assertion.*");
}

TEST(GreeterDeathTest, SegfaultsForInvalidNameArg)
{
    EXPECT_EXIT(greeterCreate((const char *)4), KilledBySignal(SIGSEGV), ".*");
}

TEST(GreeterDeathTest, SegfaultsForInvalidSelfArg)
{
    EXPECT_EXIT(greeterGreet((greeter_t *)42, "Joe Black"), KilledBySignal(SIGSEGV), ".*");
}
```

For the complete reference, see
[Death Assertions](https://google.github.io/googletest/reference/assertions.html#death)
in [[GTest Guide][]].


# Fixtures: Sharing Setup and Teardown

When testing C modules that need initialization (e.g., buffers, file
handles, or global state), writing setup and cleanup logic in every test
is error-prone. Fixtures let you centralize preparation and cleanup so
every test starts with a known state.

## Example

    GTest4C/
    ├── src/
    │   ├── greeter.c
    │   └── greeter.h
    ├── tests/
    │   └── greeter_test_fixture.cpp
    └── CMakeLists.txt

```cmake
# CMakeLists.txt (excerpt)
add_executable( greeter_test_fixture
    tests/greeter_test_fixture.cpp
    src/greeter.c
)
target_link_libraries( greeter_test_fixture ${GTEST_LIBRARIES} gmock gmock_main pthread logger )
gtest_discover_tests( greeter_test_fixture )
```

```c++
// greeter_test_fixture.cpp
#include <gtest/gtest.h>
extern "C" {
#include "greeter.h"
}

class GreeterTestFixture : public testing::Test
{
  protected:
    void SetUp() override {
        g_ = greeterCreate("Hello");
        ASSERT_NE(g_, nullptr);
    }

    void TearDown() override {
        greeterDestroy(&g_);
    }

  protected:
    greeter_t *g_;
};

/*
TEST_F(TestFixtureClassName, TestName) {
  ... test body ...
}
*/

TEST_F(GreeterTestFixture, GreetsPersonally)
{
    EXPECT_STREQ(greeterGreet(g_, "Szia, Szevasz"), "Hello, Szia, Szevasz!");
}
```


# Parameterized Testing: Avoiding Repetition

C functions are often tested with a wide variety of inputs. Writing a
separate test for each case can lead to duplicated code. Parameterized
tests allow you to define a single test logic and run it against
multiple input sets automatically.

## Example

    GTest4C/
    ├── src/
    │   ├── greeter.c
    │   └── greeter.h
    ├── tests/
    │   └── greeter_param_test.cpp
    └── CMakeLists.txt

```cmake
# CMakeLists.txt (excerpt)
add_executable( greeter_param_test
    tests/greeter_param_test.cpp
    src/greeter.c
)
target_link_libraries( greeter_param_test ${GTEST_LIBRARIES} gmock gmock_main pthread logger )
gtest_discover_tests( greeter_param_test )
```

```c++
// greeter_param_test.cpp
#include <gtest/gtest.h>
extern "C" {
#include "greeter.h"
}

typedef struct
{
    const char *name;
    const char *output;

} GreetCase;

inline void PrintTo(const GreetCase &gcase, ::std::ostream *os) {
    *os << "GreetCase{name=\"" << (gcase.name ?: "(null)")
        << "\", output=\"" << gcase.output << "\"}";
}

class GreeterParamTest : public testing::TestWithParam<GreetCase>
{
  protected:
    void SetUp() override {
        g_ = greeterCreate("Hello");
        ASSERT_NE(g_, nullptr);
    }

    void TearDown() override {
        greeterDestroy(&g_);
    }

  protected:
    greeter_t *g_;
};

/*
TEST_P(FooTest, DoesBlah) {
  // Inside a test, access the test parameter with the GetParam() method
  // of the TestWithParam<T> class:
  EXPECT_TRUE(foo.Blah(GetParam()));
  ...
}

TEST_P(FooTest, HasBlahBlah) {
  ...
}

INSTANTIATE_TEST_SUITE_P(MeenyMinyMoe,
                         FooTest,
                         testing::Values("meeny", "miny", "moe"));
*/

TEST_P(GreeterParamTest, ReturnsNonEmpty)
{
    GreetCase param = GetParam();
    const char *result = greeterGreet(g_, param.name);
    ASSERT_NE(result, nullptr);
    EXPECT_STRNE(result, "");
}

TEST_P(GreeterParamTest, GreetsAsExpected)
{
    GreetCase param = GetParam();
    EXPECT_STREQ(greeterGreet(g_, param.name), param.output);
}

INSTANTIATE_TEST_SUITE_P(
    GreeterTests,
    GreeterParamTest,
    ::testing::Values(
        GreetCase{ .name = NULL, .output = "Hello, World!" },
        GreetCase{ "World",   "Hello, World!" },
        GreetCase{ "Hello",   "Hello, Hello!" },
        GreetCase{ "Leo",     "Hello, Leo!" },
        GreetCase{ "Alice",   "Hello, Alice!" },
        GreetCase{ "Bob",     "Hello, Bob!" },
        GreetCase{ "Clarice", "Hello, Clarice!" }
    )
);
```

## Parameter Generators

```c++
Range(begin, end[, step]) // values {begin, begin+step, begin+step+step, ...}, end is not included, step defaults to 1
Values(v1, v2, ..., vN)   // values {v1, v2, ..., vN}
ValuesIn(container)       // values from a C-style array, an STL-style container
ValuesIn(begin, end)      // values in an iterator range [begin, end)
Bool()                    // sequence {false, true}
Combine(g1, g2, ..., gN)  // all n-tuple combinations (Cartesian product) of the values generated by the given n generators g1...gN
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

GMock is our friend if any of the following problems is bothering us:

- Our tests are slow as they depend on expensive resources (e.g. a database).
- Our tests are brittle as some resources they use are unreliable (e.g. the network).
- We want to test how your code handles failures but it’s not easy to cause them.
- We need to make sure that our module interacts with other modules in the right way.
- We want to “mock out” the dependencies, except that they don’t have mock implementations yet.

A __mock__ is a stand-in for a real dependency. It behaves like the
original interface but allows us to:

- Control behavior (specify what values functions return).
- Verify interactions (check how our code called the dependency).
- Avoid side effects (no files written, no network calls).


Given the original interface in C:

```c
// foo.h
typedef struct {
    int i;
    char c;
} foo_t;

foo_t *fooCreate(int i, char c);
int fooDoThis(const foo_t *self, const char *str);
void fooSetThat(foo_t *self, int arg);
bool fooIsThatSet(const foo_t *self);
void fooDestroy(foo_t **self);
```

Can be mocked as:

```c++
// foo_mock.hpp
#include <gmock/gmock.h>
#include "single.hpp"
extern "C" {
#include "foo.h"
}

class FooMock : public foo_t,
                public Single<FooMock>
{
  public:
    MOCK_METHOD(foo_t *, FooCreate, (int i, char c));
    MOCK_METHOD(int, FooDoThis, (const foo_t *self, const char *str), (const));
    MOCK_METHOD(void, FooSetThat, (foo_t *self, int val));
    MOCK_METHOD(bool, FooIsThatSet, (const foo_t *self), (const));
    MOCK_METHOD(void, FooDestroy, (foo_t **self));
};
```

```c++
// foo_mock.cpp
#include "foo_mock.hpp"
extern "C" {

foo_t *fooCreate(int i, char c) {
    return FooMock::GetInstance().FooCreate(i, c);
}

int fooDoThis(const foo_t *self, const char *str) {
    return FooMock::GetInstance().FooDoThis(self, str);
}

void fooSetThat(foo_t *self, int val) {
    return FooMock::GetInstance().FooDoThis(self, val);
}

bool fooIsThatSet(const foo_t *self) {
    return FooMock::GetInstance().FooIsThatSet(self);
}

void fooDestroy(foo_t **self) {
    return FooMock::GetInstance().FooDestroy(self);
}

} // extern "C"
```

## Mock types: The Nice, the Strict, and the Naggy

```c++
using ::testing::NiceMock;
using ::testing::NaggyMock;
using ::testing::StrictMock;

NiceMock<FooMock> nice_foo;      // Ignores unexpected calls
NaggyMock<FooMock> naggy_foo;    // Warns on unexpected calls (default)
StrictMock<FooMock> strict_foo;  // Fails on unexpected calls
```

## Return Value

- If not set explicitly, gMock uses default values
    - `0`/`false`/`nullptr` for primitive types (int, double, bool, pointers)
    - default constructed object for std lib and user defined classes
- Explicitly defined value
    - `ON_CALL` - stub behavior
    - `EXPECT_CALL` - expectation
- If neither default value nor default constructor exists for the return type: runtime error.
- Global defaults can be customized with `DefaultValue<T>`.


## Test Pattern

```c++
#include "gmock/gmock.h"
#include "foo_mock.hpp"

using ::testing::Return;

TEST(ProdTest, DoesThat)
{
  NiceMock<FooMock> foo;

  ON_CALL(foo, FooIsThatSet()).WillByDefault(Return(true));
  // ... other default actions ...

  EXPECT_CALL(foo, FooDoThis(_, 3))
      .Times(2)
      .WillOnce(Return(0)),
      .WillOnce(Return(-1));
  // ... other expectations ...

  myProdFunc(&foo), "Let it rip";
}
```

GMock evaluates expectations dynamically as the code runs, and finally
verifies that all declared expectations were met at test teardown, when
the mock object is destructed.


## Example

    GTest4C/
    ├── src/
    │   ├── greeter.c
    │   └── greeter.h
    ├── tests/
    │   └── greeter_mock_test.cpp
    ├── mock/
    │   ├── logger_mock.cpp
    │   ├── logger_mock.hpp
    │   └── single.hpp
    └── CMakeLists.txt

```cmake
# CMakeLists.txt (excerpt)
add_executable( greeter_mock_test
    tests/greeter_mock_test.cpp
    src/greeter.c
    mock/logger_mock.cpp
)
target_link_libraries( greeter_mock_test ${GTEST_LIBRARIES} gmock gmock_main pthread )
gtest_discover_tests( greeter_mock_test )
```

```c++
// logger_mock.hpp
#ifndef LOGGER_MOCK_HPP_
#define LOGGER_MOCK_HPP_

#include <gmock/gmock.h>
#include "single.hpp"
extern "C" {
#include "logger.h"
}

class LoggerMock : public Single<LoggerMock>
{
  public:
    MOCK_METHOD(int, LoggerWriteLog, (const char *message));
};

#endif  // LOGGER_MOCK_HPP_
```

```c++
// logger_mock.cpp
#include "logger_mock.hpp"

extern "C" {

int loggerWriteLog(const char *message) {
    return LoggerMock::GetInstance().LoggerWriteLog(message);
}

} // extern "C"
```

```c++
// greeter_mock_test.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
extern "C" {
#include "greeter.h"
}
#include "logger_mock.hpp"

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::AnyNumber;
using ::testing::AtLeast;
using ::testing::StrEq;
using ::testing::StrCaseEq;
using ::testing::HasSubstr;
using ::testing::Invoke;
using ::testing::InSequence;
using ::testing::Expectation;
using ::testing::_;

/*
EXPECT_CALL(mock_object, method_name (matchers...))
    .Times(cardinality)            // at most once
    .After(expectations...)        // any number of times
    .WillOnce(action)              // any number of times
    .WillRepeatedly(action)        // at most once
    .With(multi_argument_matcher)  // at most once
*/

TEST(LoggerMockTest, LoggerMockWorks)
{
    LoggerMock logger;
    EXPECT_CALL(logger, LoggerWriteLog).Times(100);

    for(int i = 0; i < 100; ++i)
        loggerWriteLog("message");
}

TEST(GreeterMockTest, CallsLogger)
{
    LoggerMock logger;
    EXPECT_CALL(logger, LoggerWriteLog(_)).Times(1);

    auto gr = greeterCreate("Hey");
    greeterGreet(gr, "You");
    greeterDestroy(&gr);
}

TEST(GreeterMockTest, CallsLoggerWithMessage)
{
    NiceMock<LoggerMock> logger;
    EXPECT_CALL(logger, LoggerWriteLog(_)).Times(AnyNumber());
    EXPECT_CALL(logger, LoggerWriteLog(HasSubstr("Siri"))).Times(AtLeast(2));

    auto h = greeterCreate("Hey");
    greeterGreet(h, "Ladies");
    greeterGreet(h, "Alexa");
    greeterGreet(h, "Siri");
    greeterGreet(h, "Ho");
    greeterGreet(h, "Siri");
    greeterDestroy(&h);
}

TEST(GreeterMockTest, CallsLoggerWithMessageInOrder)
{
    NiceMock<LoggerMock> logger;
    Expectation ladies =
        EXPECT_CALL(logger, LoggerWriteLog(StrEq("Welcome, Ladies!")));
    EXPECT_CALL(logger, LoggerWriteLog(HasSubstr("Bob"))).After(ladies);

    auto h = greeterCreate("Welcome");
    greeterGreet(h, "Ladies");
    greeterGreet(h, "Bob");
    greeterDestroy(&h);
}

TEST(GreeterTest, CallsLoggerInSequence)
{
    NiceMock<LoggerMock> logger;

    EXPECT_CALL(logger, LoggerWriteLog(_));
    {
        InSequence seq;
        EXPECT_CALL(logger, LoggerWriteLog(StrEq("Yo, Dude!")));
        EXPECT_CALL(logger, LoggerWriteLog(StrCaseEq("YO, MTV RAPS!")));
    }

    auto y = greeterCreate("Yo");
    greeterGreet(y, "Dude");
    greeterGreet(y, "mama so fat");
    greeterGreet(y, "MTV Raps");
    greeterDestroy(&y);
}

TEST(GreeterMockTest, IgnoresLoggerError)
{
    NiceMock<LoggerMock> logger;
    ON_CALL(logger, LoggerWriteLog).WillByDefault(Return(-1));

    auto oh = greeterCreate("Oh");
    EXPECT_STREQ(greeterGreet(oh, "Yeah"  ), "Oh, Yeah!");
    EXPECT_STREQ(greeterGreet(oh, "My God"), "Oh, My God!");
    EXPECT_STREQ(greeterGreet(oh, "No"    ), "Oh, No!");
    EXPECT_STREQ(greeterGreet(oh, "Dear"  ), "Oh, Dear!");

    greeterDestroy(&oh);
}

TEST(GreeterMockTest, IgnoresLoggerError2)
{
    NiceMock<LoggerMock> logger;
    int callCount = 0;

    EXPECT_CALL(logger, LoggerWriteLog(_))
        .WillRepeatedly(
            Invoke(
                [&callCount](const char *msg) {
                    return (++callCount == 2) ? -1 : 0;
                }));

    auto i = greeterCreate("I love you");
    EXPECT_STREQ(greeterGreet(i, "Pumpkin"), "I love you, Pumpkin!");
    EXPECT_STREQ(greeterGreet(i, "Honey-Bunny"), "I love you, Honey-Bunny!");
    greeterDestroy(&i);
}
```

For the complete reference, see
[Mocking Reference](https://google.github.io/googletest/reference/mocking.html),
[Matchers](https://google.github.io/googletest/reference/matchers.html) and
[Actions](https://google.github.io/googletest/reference/actions.html)
in [[GTest Guide][]].


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
build/greeter_test --gtest_output=json:results.json
```

```json
/* results.json */
{
  "tests": 1,
  "failures": 1,
  "disabled": 0,
  "errors": 0,
  "timestamp": "2025-09-22T16:42:09Z",
  "time": "0s",
  "name": "AllTests",
  "testsuites": [
    {
      "name": "GreeterTestFixture",
      "tests": 1,
      "failures": 1,
      "disabled": 0,
      "errors": 0,
      "timestamp": "2025-09-22T16:42:09Z",
      "time": "0s",
      "testsuite": [
        {
          "name": "GreetsPersonally",
          "file": "\/home\/dev\/git\/GTest4C\/tests\/greeter_test_fixture.cpp",
          "line": 29,
          "status": "RUN",
          "result": "COMPLETED",
          "timestamp": "2025-09-22T16:42:09Z",
          "time": "0s",
          "classname": "GreeterTestFixture",
          "failures": [
            {
              "failure": "\/home\/dev\/git\/GTest4C\/tests\/greeter_test_fixture.cpp:31\nExpected equality of these values:\n  greeterGreet(g_, \"Szia, Szevasz\")\n    Which is: \"Hello, Szia, Szevasz!\"\n  \"Hello, Szia, SzevasZ!\"\n",
              "type": ""
            }
          ]
        }
      ]
    }
  ]
}
```


# CTest: A Unified Test Runner

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


## Build and Run Tests

```bash
src_dir=.
build_dir=build
cov_toggle=OFF

cmake -S ${src_dir} -B ${build_dir} -DCOV=${cov_toggle}
cmake --build ${build_dir}
ctest --test-dir ${build_dir}
```


# CDash: Dashboard Intergration

CDash is a web-based dashboard system developed by Kitware (the creators
of CMake and CTest). It is designed to collect, store, and visualize the
results of software builds and tests.

[https://my.cdash.org/](https://my.cdash.org/)

```
   +-----------------+
   |   Source Code   |
   +-----------------+
            |
            v
   +-----------------+
   |      CMake      |   (configure & generate build system)
   +-----------------+
            |
            v
   +-----------------+
   |      Build      |   (compile project & tests)
   +-----------------+
            |
            v
   +-----------------+
   |      CTest      |   (run tests, coverage, memcheck)
   +-----------------+
            |
            v
   +-----------------+
   |      CDash      |   (dashboard: visualize results)
   +-----------------+
```

```cmake
# CTestConfig.cmake
set(CTEST_PROJECT_NAME "GTest4C")
set(CTEST_NIGHTLY_START_TIME "00:00:00 UTC")

set(CTEST_DROP_METHOD "https")
set(CTEST_DROP_SITE "my.cdash.org")
set(CTEST_DROP_LOCATION "/submit.php?project=${CTEST_PROJECT_NAME}")
set(CTEST_DROP_SITE_CDASH TRUE)

# Memory checking
find_program(CTEST_MEMORYCHECK_COMMAND valgrind)
set(CTEST_MEMORYCHECK_TYPE "Valgrind")
set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --error-exitcode=1")
```

## Run Tests and Submit Results

```bash
src_dir=.
cov_test_dir=ctest/cov
mem_test_dir=ctest/mem

cmake -S ${src_dir} -B ${cov_test_dir} -DCOV=ON
ctest --test-dir ${cov_test_dir} -D Experimental

cmake -S ${src_dir} -B ${mem_test_dir}
ctest --test-dir ${mem_test_dir} -D MemoryCheck
```


# References

- [GTest Guide]: https://google.github.io/googletest/
    [GTest Guide] GoogleTest's User Guide

- [GTest Code]: https://github.com/google/googletest
    [GTest Code] GoogleTest on GitHub
