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
    // comment it out for failing assert:
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
    // change their order for failing expectation:
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
