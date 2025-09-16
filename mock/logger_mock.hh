// logger_mock.hh
#ifndef LOGGER_MOCK_HH_
#define LOGGER_MOCK_HH_

#include <gmock/gmock.h>
#include "single.hh"
// extern "C" {
// #include "logger.h"
// }

class LoggerMock : public Single<LoggerMock>
{
  public:
    MOCK_METHOD(int, LoggerWriteLog, (const char *message));
};

#endif
