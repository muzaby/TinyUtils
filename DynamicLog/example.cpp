#include <chrono>
#include <cstdlib>
#include <thread>
#include "DynamicLog.h"

using LogGroup = tinyutils::LogGroup;
static LogGroup NONE = tinyutils::LogGroup::NONE;
static LogGroup CORE = tinyutils::LogGroup::CORE;

#define CORE_ERR(fmt, ...)  TLOG_ERR(CORE, fmt, ##__VA_ARGS__)
#define CORE_WARN(fmt, ...)  TLOG_WARN(CORE, fmt, ##__VA_ARGS__)
#define CORE_INFO(fmt, ...)  TLOG_INFO(CORE, fmt, ##__VA_ARGS__)
#define CORE_DEBUG(fmt, ...)  TLOG_DEBUG(CORE, fmt, ##__VA_ARGS__)

int main() {
    for (int i = 0; i < 10; i++) {
        // without wrapper
        TLOG_ERR(NONE, "hello world");
        TLOG_WARN(NONE, "hello world");
        TLOG_INFO(NONE, "hello world");
        TLOG_DEBUG(NONE, "hello world");

        // with wrapper
        CORE_ERR("hello world");
        CORE_WARN("hello world");
        CORE_INFO("hello world");
        CORE_DEBUG("hello world");

        // put enough delay to override flag
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
