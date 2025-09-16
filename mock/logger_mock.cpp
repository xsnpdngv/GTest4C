// logger_mock.cpp
#include "logger_mock.hh"

extern "C"
{

int loggerWriteLog(const char *message) {
    return LoggerMock::GetInstance().LoggerWriteLog(message);
}

} // extern "C"
