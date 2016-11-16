#ifndef logging_h
#define logging_h

#include <stdio.h>
#include <inttypes.h>

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

#define DLOG(format, ...) \
    if (_log_level <= DEBUG) \
        fprintf(_log_fildes, "[DEBUG]%s:%s:%d "format"\n", \
            __FILE__, __func__, __LINE__, ##__VA_ARGS__);

#define ILOG(format, ...) \
    if (_log_level <= INFO) \
        fprintf(_log_fildes, "[INFO]%s:%s:%d "format"\n", \
            __FILE__, __func__, __LINE__, ##__VA_ARGS__);

#define WLOG(format, ...) \
    if (_log_level <= WARN) \
        fprintf(_log_fildes, "[WARN]%s:%s:%d "format"\n", \
            __FILE__, __func__, __LINE__, ##__VA_ARGS__);

#define ELOG(format, ...) \
    if (_log_level <= ERROR) \
        fprintf(_log_fildes, "[ERROR]%s:%s:%d "format"\n", \
            __FILE__, __func__, __LINE__, ##__VA_ARGS__);

enum LOG_LEVEL { DEBUG, INFO, WARN, ERROR, SILENT };

extern enum LOG_LEVEL _log_level;
extern FILE *_log_fildes;

void init_logging(enum LOG_LEVEL level, FILE *fildes);

#endif /* logging_h */
