/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */

#include "unistd.h"
#include "string.h"
#include "stdlib.h"

int
main(int argc, const char* argv[])
{
    unsigned long stdout = 1;
    char cr = '\n';

    for (int i = 1; i < argc; i++)
    {
        const char *str = argv[i];
        size_t len = strlen(str);
        write(stdout, str, len);
        char space = ' ';
        if (i < argc - 1)
            write(stdout, &space, 1);
    }

    write(stdout, &cr, 1);

    //__asm__("vmcall;\n");
    return 0;
}
