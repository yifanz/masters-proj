/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */
#include <dlfcn.h>

#include "plugin.h"
#include "conf.h"
#include "logging.h"

static struct plugin_ops _ops = {0};

const struct plugin_ops*
get_plugin_ops()
{
    return &_ops;
}

int
load_plugin(const char *path)
{
    if (path == NULL)
    {
        goto ERROR;
    }

    void *plugin = dlopen(path, RTLD_LOCAL|RTLD_NOW);

    if (plugin == NULL)
    {
        ELOG("open plugin %s failed", path);
        goto ERROR;
    }

    int (*plugin_init)(struct plugin_ops*) = dlsym(plugin,
            "plugin_init");

    if (plugin_init == NULL)
    {
        ELOG("plugin_init not defined");
        dlclose(plugin);
        goto ERROR;
    }

    if (plugin_init(&_ops))
    {
        ELOG("plugin_init failed");
        dlclose(plugin);
        goto ERROR;
    }

    return 0;

ERROR:
    return -1;
}
