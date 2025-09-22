// greeter_mock_test.cpp
#include <gtest/gtest.h>
extern "C" {
#include "greeter.h"
}
#include "logger_mock.hpp"

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnArg;
using ::testing::AnyNumber;
using ::testing::AtLeast;
using ::testing::ResultOf;
using ::testing::Eq;
using ::testing::StrEq;
using ::testing::MatchesRegex;
using ::testing::Pointee;
using ::testing::Pointer;
using ::testing::AllOf;
using ::testing::NotNull;
using ::testing::Invoke;
using ::testing::InSequence;
using ::testing::_;

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
    EXPECT_CALL(logger, LoggerWriteLog(MatchesRegex(".*Siri.*"))).Times(AtLeast(2));

    auto h = greeterCreate("Hey");
    greeterGreet(h, "You");
    greeterGreet(h, "Alexa");
    greeterGreet(h, "Siri");
    greeterGreet(h, "Ho");
    greeterGreet(h, "Siri");

    greeterDestroy(&h);
}

TEST(GreeterTest, CallsLoggerInOrder)
{
    NiceMock<LoggerMock> logger;

    EXPECT_CALL(logger, LoggerWriteLog(_));
    {
        InSequence seq;
        EXPECT_CALL(logger, LoggerWriteLog(StrEq("Yo, Dude!")));
        EXPECT_CALL(logger, LoggerWriteLog(StrEq("Yo, MTV Raps!")));
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
