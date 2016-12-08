/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */
#ifndef emu_syscall_h
#define emu_syscall_h

#include <stdint.h>

// If 1 then pause for user input on syscalls.
extern int syscall_interactive;

/**
 * System V syscall ABI 64-bit
 *
 * Arguments are passed in rdi, rsi, rdx, r10, r8 and r9.
 * Syscall number stored in the rax register.
 * The call is executed using the 'syscall' instruction.
 */
int emu_syscall(hv_vcpuid_t vcpu);

#endif /* emu_syscall_h */
