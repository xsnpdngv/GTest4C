// greeter_test.cpp
#include <gtest/gtest.h>
extern "C" {
#include "greeter.h"
}


typedef struct
{
    const char *name;
    const char *output;

} GreetCase;

inline void PrintTo(const GreetCase& gcase, ::std::ostream* os) {
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
