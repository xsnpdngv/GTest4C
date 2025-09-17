#include <gtest/gtest.h>
extern "C" {
#include "greeter.h"
#include "logger.h"
}
#include "logger_mock.hh"


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
}

TEST(GreeterMockTest, CallsLoggerWithMessage)
{
    NiceMock<LoggerMock> logger;
    EXPECT_CALL(logger, LoggerWriteLog(_)).Times(AnyNumber());
    EXPECT_CALL(logger, LoggerWriteLog(MatchesRegex("Hey..Siri."))).Times(2);

    auto hey = greeterCreate("Hey");
    greeterGreet(hey, "You");
    greeterGreet(hey, "Alexa");
    greeterGreet(hey, "Siri");
    greeterGreet(hey, "Ho");
    greeterGreet(hey, "Siri");

    greeterDestroy(&hey);
}

TEST(GreeterMockTest, IgnoresLoggerError)
{
    NiceMock<LoggerMock> logger;
    ON_CALL(logger, LoggerWriteLog).WillByDefault(Return(-1));

    auto oh = greeterCreate("Oh");
    EXPECT_STREQ(greeterGreet(oh, "Yeah"), "Oh, Yeah!");
    EXPECT_STREQ(greeterGreet(oh, "My God"), "Oh, My God!");
    EXPECT_STREQ(greeterGreet(oh, "No"), "Oh, No!");
    EXPECT_STREQ(greeterGreet(oh, "Dear"), "Oh, Dear!");

    greeterDestroy(&oh);
}
