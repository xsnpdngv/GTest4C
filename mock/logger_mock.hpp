// logger_mock.hpp
#ifndef LOGGER_MOCK_HPP_
#define LOGGER_MOCK_HPP_

#include <gmock/gmock.h>
#include "single.hpp"
extern "C" {
#include "logger.h"
}

class LoggerMock : public Single<LoggerMock>
{
  public:
    MOCK_METHOD(int, LoggerWriteLog, (const char *message));
};

#endif  // LOGGER_MOCK_HPP_
