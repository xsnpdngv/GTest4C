% GoogleTest for Testing C Code  
% Tamás Dezső  
% Sept 15, 2025  

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


# Introduction

GoogleTest (GTest) is one of the most widely used frameworks for
unit testing in C++ projects. While designed for C++, it can also be
applied to C codebases, enabling developers to take advantage of its
expressive assertions, test fixtures, and integration with build and
CI tools. This article explains how to set up GTest for C projects,
demonstrates practical techniques for testing C functions, and
highlights how mocking can improve testability.


# What You’ll Gain

By reading this article and following the examples, you will learn how to:

- Write clear and maintainable tests for C code using GoogleTest.
- Use assertions to express expected behavior.
- Apply fixtures to share setup and teardown logic across multiple tests.
- Create parameterized tests to cover multiple input combinations efficiently.
- Mock dependencies to test modules in isolation.
- Use death tests to detect crashes, aborts, and segmentation faults safely.
- Integrate tests into CI/CD pipelines with JSON output for automated reporting.

This article walks you step-by-step through practical examples and
provides a ready-to-clone project structure.


# Why Use GTest for C Code?

GTest offers a powerful, modern unit testing environment even for C projects:

- __Mature and well-supported__: battle-tested and integrated with CI/CD pipelines.
- __Rich feature set__: assertions, fixtures, parameterized tests, death tests, and mocking.
- __Organized tests__: group tests into suites, run selectively, and share setup/teardown.
- __Maintainable reporting__: human-readable output, JSON/XML for CI, clear failure messages.
- __Cross-platform__: works on Linux, macOS, Windows, and embedded systems.
- __Mocking support__: with GoogleMock (gMock), for testing interactions with dependencies.

By adding a thin C++ layer, all of these strengths can be applied directly to C modules.


# Setting Up

To use GTest for C code, the followings are needed:

1.	GoogleTest installed (via package manager or as a submodule in your project).
2.	CMake or Makefile configuration that links C modules into a C++ test executable.
3.	C headers wrapped with extern "C" when included in C++ files, so that function names are not mangled.


# Asserts: Expressing Expectations Clearly

Assertions are the building blocks of unit testing. They define the
expected behavior of your code and provide meaningful error messages
when the behavior deviates. GTest offers a rich set of assertions that
go far beyond simple equality checks.

GTest distinguishes between:

- `EXPECT_*`: non-fatal assertions: the test continues after a failure.
- `ASSERT_*`: fatal assertions: the test aborts immediately on failure.

This distinction allows precise control over test flow and error reporting.


# Fixtures: Sharing Setup and Teardown

When testing C modules that need initialization (e.g., buffers, file
handles, or global state), writing setup and cleanup logic in every test
is error-prone. Fixtures let you centralize preparation and cleanup so
every test starts with a known state.

## Example


# Parameterized Testing: Avoiding Repetition

C functions are often tested with a wide variety of inputs. Writing a
separate test for each case can lead to duplicated code. Parameterized
tests allow you to define a single test logic and run it against
multiple input sets automatically.

## Example


# Mocking: Isolating Dependencies

In C projects, functions often depend on external resources (files,
sockets, hardware APIs). To test reliably, we want to replace real
dependencies with controlled test doubles. While C doesn’t have built-in
mocking, GTest (via GoogleMock) provides a way to mock functions called
from C modules by declaring them in C++.

## Example


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
extern "C" {
void doHarakiri() { int *p = NULL; *p = 42; }
}

TEST(DeathTest, SegfaultDetected) {
    EXPECT_EXIT(doHarakiri(), testing::KilledBySignal(15), ".*");
}
```

For the complete reference, see
[Death Assertions](https://google.github.io/googletest/reference/assertions.html#death).


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



# Build

```bash
mkdir build
cd build
cmake ..
make
make test
```


# References

- [GTest Guide]: https://google.github.io/googletest/
    [GTest Guide] GoogleTest's User Guide

- [GTest Code]: https://github.com/google/googletest
    [GTest Code] GoogleTest on GitHub
