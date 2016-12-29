/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */
#include <stdio.h>

#include "hvexec_plugin.h"

int
cb_write(int fildes, const void *buf, size_t nbyte, ssize_t *ret)
{
    printf("Write called\n");
    return 1;
}

int
plugin_init(struct plugin_ops *ops)
{
    printf("Plugin Init\n");

    ops->cb_write = cb_write;

    return 0;
}
