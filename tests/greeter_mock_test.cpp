#include <gtest/gtest.h>
extern "C" {
#include "greeter.h"
}
#include "logger_mock.hh"


using ::testing::NiceMock;
using ::testing::_;
using ::testing::StrEq;
using ::testing::Return;
using ::testing::AnyNumber;


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
    EXPECT_CALL(logger, LoggerWriteLog(StrEq("Hey, You!"))).Times(2);

    auto gr = greeterCreate("Hey");
    greeterGreet(gr, "Siri");
    greeterGreet(gr, "You");
    greeterGreet(gr, "Man");
    greeterGreet(gr, "You");
}

TEST(GreeterMockTest, IgnoresLoggerError)
{
    NiceMock<LoggerMock> logger;
    ON_CALL(logger, LoggerWriteLog).WillByDefault(Return(-1));

    auto g = greeterCreate("Oh");
    EXPECT_STREQ(greeterGreet(g, "Yeah"), "Oh, Yeah!");
}
