// greeter_test.cpp
#include <gtest/gtest.h>
extern "C" {
#include "greeter.h"
}


// Basic Assertions /////////////////////////////

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
    EXPECT_STREQ(greeterGreet(g, "Sunshine"), "Good Morning, Sunshine!");
    greeterDestroy(&g);

    g = greeterCreate("Bonjour");
    EXPECT_STREQ(greeterGreet(g, "Alice"), "Bonjour, Alice!");
    EXPECT_STREQ(greeterGreet(g, "Bob"), "Bonjour, Bob!");
    greeterDestroy(&g);
}
