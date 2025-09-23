// logger_mock.cpp
#include "logger_mock.hpp"

LoggerMock::LoggerMock() { // change default behavior of methods if needed:
    ON_CALL(*this, LoggerWriteLog).WillByDefault(::testing::Return(42));
}

extern "C" {

int loggerWriteLog(const char *message) {
    return LoggerMock::GetInstance().LoggerWriteLog(message);
}

} // extern "C"
