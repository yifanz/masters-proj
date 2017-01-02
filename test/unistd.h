/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */

#ifndef unistd_h
#define unistd_h

#include "sys/_types/_size_t.h"
#include "sys/_types/_ssize_t.h"

ssize_t
read(int fildes, const void *buf, size_t nbyte)
{
    long code = 0x2000003;
    unsigned long ret = 0;

    __asm__(/*
               "mov $0xcafe, %%rax;\n"
               "vmcall;\n"
               */
            "movq %1, %%rax;\n"
            "movq %2, %%rdi;\n"
            "movq %3, %%rsi;\n"
            "movq %4, %%rdx;\n"
            "syscall;\n"
            "movq %%rax, %0;\n"
            : "=g"(ret)
            : "g"(code), "g"(fildes), "g"(buf), "g"(nbyte)
            : );

    return ret;
}

ssize_t
write(int fildes, const void *buf, size_t nbyte)
{
    long code = 0x2000004;
    unsigned long ret = 0;

    __asm__(/*
               "mov $0xcafe, %%rax;\n"
               "vmcall;\n"
               */
            "movq %1, %%rax;\n"
            "movq %2, %%rdi;\n"
            "movq %3, %%rsi;\n"
            "movq %4, %%rdx;\n"
            "syscall;\n"
            "movq %%rax, %0;\n"
            : "=g"(ret)
            : "g"(code), "g"(fildes), "g"(buf), "g"(nbyte)
            : );

    return ret;
}

#endif /* unistd_h */
