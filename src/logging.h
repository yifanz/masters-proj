/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */
#ifndef logging_h
#define logging_h

#include <stdio.h>
#include <inttypes.h>

/*
#define TERM_COLOR_RED     "\x1b[31m"
#define TERM_COLOR_GREEN   "\x1b[32m"
#define TERM_COLOR_YELLOW  "\x1b[33m"
#define TERM_COLOR_BLUE    "\x1b[34m"
#define TERM_COLOR_MAGENTA "\x1b[35m"
#define TERM_COLOR_CYAN    "\x1b[36m"
#define TERM_COLOR_RESET   "\x1b[0m"
*/

#define LOG(level, format, ...) \
    if (_log_level <= level) \
        fprintf(_log_fildes, "[%5s %s:%s:%d] "format"\n", \
            #level, __FILE__, __func__, __LINE__, ##__VA_ARGS__);

#define DLOG(format, ...) LOG(DEBUG, format, ##__VA_ARGS__);
#define ILOG(format, ...) LOG(INFO, format, ##__VA_ARGS__);
#define WLOG(format, ...) LOG(WARN, format, ##__VA_ARGS__);
#define ELOG(format, ...) LOG(ERROR, format, ##__VA_ARGS__);

enum LOG_LEVEL { DEBUG, INFO, WARN, ERROR, SILENT };

extern enum LOG_LEVEL _log_level;
extern FILE *_log_fildes;

void init_logging(enum LOG_LEVEL level, FILE *fildes);

#endif /* logging_h */
