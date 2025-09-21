// logger_mock.cpp
#include "logger_mock.hpp"

extern "C" {

int loggerWriteLog(const char *message) {
    return LoggerMock::GetInstance().LoggerWriteLog(message);
}

} // extern "C"
