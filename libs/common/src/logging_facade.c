
#include "logging_facade.h"
#include <stdarg.h>
#include <stdio.h>

static LoggingCallback internalCallback;

const char *lfLogLevelStr(LfLogLevel level)
{
    switch (level)
    {
        case LF_LEVEL_DEBUG:
            return "DEBUG";
        case LF_LEVEL_INFO:
            return "INFO";
        case LF_LEVEL_WARN:
            return "WARN";
        case LF_LEVEL_ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

#ifdef _WIN32
#define FILE_SEPARATOR '\\'
#else
#define FILE_SEPARATOR '/'
#endif

void lfRegister(LoggingCallback callback)
{
    internalCallback = callback;
}

static void printfCallback(LfLogLevel level,
                           const char *file,
                           int fileLen,
                           int line,
                           const char *msg,
                           va_list args)
{
    printf("[%5s](%25s:%4d) : ", lfLogLevelStr(level), file, line);
    vprintf(msg, args);
    printf("\n");
}

void lfLog(LfLogLevel level, const char *file, int fileLen, int line, const char *msg, ...)
{
    // char *fileSep = strrchr(file, FILE_SEPARATOR);
    // long index = fileSep == nullptr ? 0 : (fileSep - file);
    // file += (index + 1);

    if (internalCallback == nullptr)
    {
        va_list args;
        va_start(args, msg);
        printfCallback(level, file, fileLen, line, msg, args);
        va_end(args);
        return;
    }

    va_list args;
    va_start(args, msg);
    internalCallback(level, file, fileLen, line, msg, args);
    va_end(args);
}
