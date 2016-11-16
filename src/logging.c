#include "logging.h"

enum LOG_LEVEL _log_level = DEBUG;
FILE *_log_fildes = 0;

void
init_logging(enum LOG_LEVEL level, FILE *fildes)
{
    _log_level = level;
    _log_fildes = fildes;
}
