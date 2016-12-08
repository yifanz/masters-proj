/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */
#ifndef loader_h
#define loader_h

#include <stdint.h>

int load_raw(const char *path, uint64_t gpa);
int load_mach_obj(const char *path, uint64_t *entryoff);

#endif /* loader_h */
