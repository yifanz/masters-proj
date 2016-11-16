#ifndef emu_syscall_h
#define emu_syscall_h

#include <stdint.h>

/**
 * System V syscall ABI 64-bit
 *
 * Arguments are passed in rdi, rsi, rdx, r10, r8 and r9.
 * Syscall number stored in the rax register.
 * The call is executed using the 'syscall' instruction.
 */
int emu_syscall(hv_vcpuid_t vcpu);

#endif /* emu_syscall_h */
