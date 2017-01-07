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

    int fd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);

    if (fd < 0) {
        write(1, argv[1], strlen(argv[1]));
        const char *err_msg = ":No such file or directory\n";
        write(1, err_msg, strlen(err_msg));
        return -1;
    }

    int nbytes = atoi(argv[2]);

    for(int i = 0; i < nbytes; i++)
    {
        char c = '1';
        write(fd, &c, 1);
    }

    close(fd);

    return 0;
}
