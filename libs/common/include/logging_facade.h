
#ifndef LOGGING_FACADE_H
#define LOGGING_FACADE_H

#include <stdarg.h>
#include <stddef.h>

typedef enum
{
    LF_LEVEL_DEBUG = 0,
    LF_LEVEL_INFO,
    LF_LEVEL_NOTICE,
    LF_LEVEL_WARN,
    LF_LEVEL_ERROR,
    LF_LEVEL_FATAL
} LfLogLevel;

typedef void (*LoggingCallback)(LfLogLevel level,
                                const char *file,
                                int fileLen,
                                int line,
                                const char *msg,
                                va_list args);

void lfLog(LfLogLevel level,
           const char *file,
           int fileLen,
           int line,
           const char *msg,
           ...) __attribute__((format(__printf__, 5, 6)));

void lfRegister(LoggingCallback callback);

#define lfLogDebug(MSG, ...) lfLog(LF_LEVEL_DEBUG, __FILE__, sizeof(__FILE__) - 1, __LINE__, MSG, ##__VA_ARGS__)
#define lfLogInfo(MSG, ...) lfLog(LF_LEVEL_INFO, __FILE__, sizeof(__FILE__) - 1, __LINE__, MSG, ##__VA_ARGS__)
#define lfLogWarn(MSG, ...) lfLog(LF_LEVEL_WARN, __FILE__, sizeof(__FILE__) - 1, __LINE__, MSG, ##__VA_ARGS__)
#define lfLogError(MSG, ...) lfLog(LF_LEVEL_ERROR, __FILE__, sizeof(__FILE__) - 1, __LINE__, MSG, ##__VA_ARGS__)

#endif // LOGGING_FACADE_H
