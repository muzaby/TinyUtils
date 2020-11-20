#pragma once

#include <cstdint>
#include "TinyUtilsDefs.h"

TINYUTILS_NAMESPACE_BEGIN

struct LogInfo;
extern struct LogInfo g_logInfo;

typedef enum
{
    // Add as you like
    ERROR = 0,
    WARN,
    INFO,
    DEBUG,
    MAX,
} LogLevel;

typedef enum
{
    // Add as you like
    NONE = (1 << 0),
    CORE = (1 << 1),
} LogGroup;

struct LogInfo
{
    std::uint32_t flag[LogLevel::MAX];
    int fd;
};

class DynamicLog
{
public:
    static constexpr const char* FIFOPath = "./tinyutils_dynmaiclog_pipe";
    static constexpr std::size_t MaxLogLength = 1024;
    static const char* Level2String(LogLevel level);
    static const char* Group2String(LogGroup group);
    static void Log(const char* fmt, ...);

private:
    DynamicLog() = default;
    DynamicLog(const DynamicLog&) = delete;
    DynamicLog& operator=(const DynamicLog&) = delete;
};

#define TINY_LOG(level, group, fmt, ...) \
    tinyutils::DynamicLog::Log( \
        "[%s][%s] %s:%u %s() " fmt "\n", \
        tinyutils::DynamicLog::Level2String(level), \
        tinyutils::DynamicLog::Group2String(group), \
        __FILE__, \
        __LINE__, \
        __func__, \
        ##__VA_ARGS__)

#define TLOG_ERR(group, fmt, ...) \
    do { \
        if (tinyutils::g_logInfo.flag[tinyutils::LogLevel::ERROR] & group) { \
            TINY_LOG(tinyutils::LogLevel::ERROR, group, fmt, ##__VA_ARGS__); \
        } \
    } while (0)

#define TLOG_WARN(group, fmt, ...) \
    do { \
        if (tinyutils::g_logInfo.flag[tinyutils::LogLevel::WARN] & group) { \
            TINY_LOG(tinyutils::LogLevel::WARN, group, fmt, ##__VA_ARGS__); \
        } \
    } while (0)

#define TLOG_INFO(group, fmt, ...) \
    do { \
        if (tinyutils::g_logInfo.flag[tinyutils::LogLevel::INFO] & group) { \
            TINY_LOG(tinyutils::LogLevel::INFO, group, fmt, ##__VA_ARGS__); \
        } \
    } while (0)

#define TLOG_DEBUG(group, fmt, ...) \
    do { \
        if (tinyutils::g_logInfo.flag[tinyutils::LogLevel::DEBUG] & group) { \
            TINY_LOG(tinyutils::LogLevel::DEBUG, group, fmt, ##__VA_ARGS__); \
        } \
    } while (0)

TINYUTILS_NAMESPACE_END
