/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */
#ifndef hvexec_plugin_h
#define hvexec_plugin_h

#include <unistd.h>

struct plugin_ops
{
    int (*cb_write)(int, const void*, size_t, ssize_t*);
    int (*cb_read)(int, const void*, size_t, ssize_t*);
};

int plugin_init(struct plugin_ops *ops);

#endif /* hvexec_plugin_h */
