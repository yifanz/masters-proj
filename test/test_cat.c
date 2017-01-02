/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */

#include "sys/fcntl.h"
#include "unistd.h"
#include "string.h"
#include "stdlib.h"

int
main(int argc, const char* argv[])
{
    int ret = 0;

    if (argc < 2) return -1;

    int fd = open(argv[1], O_RDONLY, 0);

    if (fd < 0) {
        write(1, argv[1], strlen(argv[1]));
        const char *err_msg = ":No such file or directory\n";
        write(1, err_msg, strlen(err_msg));
        return -1;
    }

    char buf[128] = {0};
    ssize_t nbytes;

    while(1)
    {
        nbytes = read(fd, buf, sizeof(buf));
        if (nbytes <= 0) {
            break;
        }
        write(1, buf, nbytes);
    }

    return ret;
}
