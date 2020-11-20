#include <cstdarg>
#include <cstring>
#include <thread>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "DynamicLog.h"

TINYUTILS_NAMESPACE_BEGIN

#define LOGE(fmt, ...)  TLOG_ERR(NONE, fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...)  TLOG_WARN(NONE, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...)  TLOG_INFO(NONE, fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...)  TLOG_DEBUG(NONE, fmt, ##__VA_ARGS__)

static int InitLogFD();
static bool InitLogSystem();

struct LogInfo g_logInfo = {
    .flag = {
        0xf, // ERROR
        0xf, // WARN
        0xf, // INFO
        0xf, // DEBUG
    },
    .fd = InitLogFD(),
};

static bool g_initDone = InitLogSystem();

int InitLogFD()
{
#if defined(LOG2FILE)
    int fd = open("./log.txt", O_WRONLY | O_CREAT);
    if (fd < 0) {
        LOGE("error : %d", errno);
        return 1;
    }
    return fd;
#else
    return 1;
#endif
}

const char* DynamicLog::Level2String(
    LogLevel level)
{
    switch (level) {
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::WARN:
            return "WARN";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::DEBUG:
            return "DEBUG";
    };
    return "INVALID_LOGLEVEL";
}

const char* DynamicLog::Group2String(
    LogGroup group)
{
    switch (group) {
        case NONE:
            return "NONE";
        case CORE:
            return "CORE";
    };
    return "INVALID_LOGGROUP";
}

static bool String2Level(
    const char* levelString,
    LogLevel* pLevel)
{
    bool ret = false;
    static const char* levelstring[] = {
        "DynamicLog_ERROR",
        "DynamicLog_WARN",
        "DynamicLog_INFO",
        "DynamicLog_DEBUG",
    };
    static const LogLevel levelint[] = {
        LogLevel::ERROR,
        LogLevel::WARN,
        LogLevel::INFO,
        LogLevel::DEBUG,
    };
    std::size_t length = sizeof(levelstring) / sizeof(levelstring[0]);

    for (std::size_t i = 0; i < length; i++) {
        if (strcmp(levelString, levelstring[i]) == 0) {
           *pLevel = levelint[i];
           ret = true;
           break;
        }
    }

    return ret;
}

static bool IsHexDigit(
    const char* hexString)
{
    if (hexString[0] == '0' && hexString[1] == 'x')
        hexString += 2;

    while (*hexString) {
        if (std::isxdigit(*hexString++) == 0)
            return false;
    }

    return true;
}

static void OverrideFlag(
    const char* overridenFlag)
{
    char flagbuffer[DynamicLog::MaxLogLength];
    char* pchar;
    std::size_t length = std::strlen(overridenFlag);

    std::snprintf(flagbuffer, sizeof(flagbuffer), "%s", overridenFlag);
    pchar = std::strchr(flagbuffer, '=');

    if (pchar && (*pchar != flagbuffer[length - 1])) {
        const char* levelstring = flagbuffer;
        const char* flagstring = pchar + 1;
        *pchar = 0;
        LogLevel level;

        if (String2Level(levelstring, &level) == true) {
            if (IsHexDigit(flagstring) == true) {
                std::uint32_t flag = std::strtol(flagstring, nullptr, 16);
                g_logInfo.flag[level] = flag;
                LOGI("Override log flag : %s = %xh",
                    DynamicLog::Level2String(level), flag);
            } else {
                LOGE("Invalid log flag : %s", flagstring);
            }
        } else {
            LOGE("Invalid log level : %s", levelstring);
        }
    } else {
        LOGE("Invalid input : %s", overridenFlag);
    }
}

static void GenerateNamedPipe()
{
    int ret = mkfifo(DynamicLog::FIFOPath, 0666);
    if (ret < 0) {
        switch (errno) {
            case EEXIST:
                LOGW("fifo already exists : %s",
                    DynamicLog::FIFOPath);
                break;
            default:
                LOGE("mkfifo() fails : %d", errno);
                break;
        }
    }
}

bool InitLogSystem()
{
    GenerateNamedPipe();
    std::thread([=]() {
        char input[DynamicLog::MaxLogLength];
        while (true) {
            FILE* fifo = std::fopen(DynamicLog::FIFOPath, "r");
            if (!fifo) {
                LOGE("cannot find fifo");
                return;
            }

            std::size_t numBytes
                = std::fread(input, sizeof(input[0]), DynamicLog::MaxLogLength, fifo);
            if (numBytes <= 0) {
                LOGE("an error occured while reading input stream. " \
                    "errono : %d", errno);
                std::fclose(fifo);
                break;
            } else if (strncmp(input, "exit", 4) == 0) {
                LOGI("Exit");
                std::fclose(fifo);
                break;
            }

            input[numBytes] = 0;
            const char* delim = " \r\n";
            char* token = std::strtok(input, delim);
            while (token) {
                OverrideFlag(token);
                token = std::strtok(nullptr, delim);
            }

            std::fclose(fifo);
        }
    }).detach();

    return true;
}

void DynamicLog::Log(
    const char* fmt,
    ...)
{
    char fmtstring[DynamicLog::MaxLogLength];
    std::va_list args;

    va_start(args, fmt);
    vsnprintf(fmtstring, sizeof(fmtstring), fmt, args);
    va_end(args);

    dprintf(g_logInfo.fd, "%s", fmtstring);
}

TINYUTILS_NAMESPACE_END
