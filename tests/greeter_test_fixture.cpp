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
