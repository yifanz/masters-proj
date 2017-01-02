/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */
#ifndef fcntl_h
#define fcntl_h

// flag
#define O_RDONLY        0x0000          /* open for reading only */
#define O_WRONLY        0x0001          /* open for writing only */
#define O_RDWR          0x0002          /* open for reading and writing */
#define O_NONBLOCK      0x0004          /* no delay */
#define O_APPEND        0x0008          /* set append mode */
#define O_CREAT         0x0200          /* create if nonexistant */
#define O_TRUNC         0x0400          /* truncate to zero length */
#define O_EXCL          0x0800          /* error if already exists */
#define O_SHLOCK        0x0010          /* open with shared file lock */
#define O_EXLOCK        0x0020          /* open with exclusive file lock */
#define O_NOFOLLOW      0x0100          /* don't follow symlinks */
#define O_SYMLINK       0x200000        /* allow open of a symlink */
#define O_EVTONLY       0x8000          /* descriptor requested for event notifications only */
#define O_CLOEXEC       0x1000000       /* implicitly set FD_CLOEXEC */

// mode
#define S_IRWXU 0000700    /* RWX mask for owner */
#define S_IRUSR 0000400    /* R for owner */
#define S_IWUSR 0000200    /* W for owner */
#define S_IXUSR 0000100    /* X for owner */

#define S_IRWXG 0000070    /* RWX mask for group */
#define S_IRGRP 0000040    /* R for group */
#define S_IWGRP 0000020    /* W for group */
#define S_IXGRP 0000010    /* X for group */

#define S_IRWXO 0000007    /* RWX mask for other */
#define S_IROTH 0000004    /* R for other */
#define S_IWOTH 0000002    /* W for other */
#define S_IXOTH 0000001    /* X for other */

#define S_ISUID 0004000    /* set user id on execution */
#define S_ISGID 0002000    /* set group id on execution */
#define S_ISVTX 0001000    /* save swapped text even after use */

int
open(const char *path, int oflag, int mode)
{
    long code = 0x2000005;
    unsigned long ret = 0;
    int set_mode = 0;

    if (oflag & O_CREAT)
    {
        set_mode = mode;
    }

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
            : "g"(code), "g"(path), "g"(oflag), "g"(set_mode)
            : );

    // TODO seems raw syscall for open returns 2 upon error
    if (ret == 2) return -1;

    return ret;
}

#endif /* fcntl_h */
