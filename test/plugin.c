/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "hvexec_plugin.h"

int
cb_write(int fildes, const void *buf, size_t nbyte, ssize_t *ret)
{
    //printf("Write called\n");
    return 1;
}

int
cb_open(const char *path, int flags, int mode, int *ret)
{
    char *full_path;
    char *private = "/Users/nlogn/Documents/hvexec/ATTRIB";
    char *redirect = "/Users/nlogn/Documents/hvexec/LICENSE";

    //printf("open:%s\n", path);
    full_path = realpath(path, NULL);

    if (full_path && strncmp(full_path, private, strlen(private)) == 0)
    {
        *ret = open(redirect, flags, mode);
        free(full_path);

        return 0;
    }
    else
    {
        return 1;
    }
}

int
plugin_init(struct plugin_ops *ops)
{
    //printf("Plugin Init\n");

    ops->cb_write = cb_write;
    ops->cb_open = cb_open;

    return 0;
}
