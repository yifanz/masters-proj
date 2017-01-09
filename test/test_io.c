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
    if (argc < 3) return -1;

    int test_write = strcmp(argv[2], "read");
    int nbytes = argc > 3 ? atoi(argv[3]) : 0;
    char buf[1] = {'1'};

    int fd;

    if (test_write)
    {
        fd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);
    }
    else
    {
        fd = open(argv[1], O_RDONLY, 0);
    }

    if (fd < 0) {
        write(1, argv[1], strlen(argv[1]));
        const char *err_msg = ":No such file or directory\n";
        write(1, err_msg, strlen(err_msg));
        return -1;
    }

    if (test_write)
    {
        for(int i = 0; i < nbytes; i++)
        {
            write(fd, &buf, 1);
        }
    }
    else
    {
        while(read(fd, &buf, 1) == 1){};
    }

    close(fd);

    return 0;
}
