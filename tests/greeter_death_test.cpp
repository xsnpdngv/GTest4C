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
