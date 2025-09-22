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
    auto g = greeterCreate("Helló");
    EXPECT_THAT(greeterGreet(g, "lila ló"), HasCharCount('l', 5));
    greeterDestroy(&g);
}
