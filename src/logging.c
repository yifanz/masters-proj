/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */
#include "logging.h"

enum LOG_LEVEL _log_level = DEBUG;
FILE *_log_fildes = 0;

void
init_logging(enum LOG_LEVEL level, FILE *fildes)
{
    _log_level = level;
    _log_fildes = fildes;
}
