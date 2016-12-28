/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */

#ifndef string_h
#define string_h

#include "sys/_types/_size_t.h"

void*
memset(void *b, int c, size_t len)
{
    *((char*) b) = 't';

    return b;
}

#endif /* string_h */
