/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */
#ifndef plugin_h
#define plugin_h

#include "hvexec_plugin.h"

const struct plugin_ops *get_plugin_ops();

int load_plugin(const char* path);

#endif /* plugin_h */
