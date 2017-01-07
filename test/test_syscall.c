/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */
#include "stdlib.h"
#include "unistd.h"
#include "string.h"

int main(int argc, char* argv[])
{
    char buf = '0';
    int n;

    if (argc > 1) n = atoi(argv[1]);
    else n = 1;

    for (int i = 0; i < n; i++)
    {
        write(1, &buf, sizeof buf);
    }

    return 0;
}
