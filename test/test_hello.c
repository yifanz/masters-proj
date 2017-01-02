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
    unsigned long ret = 0;

    //char * str = "Hello World\n";
    char * str2 = "Hello UCLA\n";
    char * str3 = "This is string 3!\n";
    char buf[20] = {0};
    unsigned long len = 0;
    
    for (int i = 0; i < argc; i++)
    {
        const char *str = argv[i];
        size_t len = strlen(str);
        write(stdout, str, len);
    }

    //__asm__("vmcall;\n");
    ret = write(stdout, str2, strlen(str2));
    itoa(ret, buf, 10);
    len = strlen(buf);
    buf[len] = '\n';
    ret = write(stdout, buf, len+1);
    ret = write(stdout, str3, strlen(str3));

    return ret;
}
