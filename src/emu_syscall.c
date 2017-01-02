/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */
#include <Hypervisor/hv.h>
#include <Hypervisor/hv_vmx.h>
#include <unistd.h>
#include <fcntl.h>

#include "emu_syscall.h"
#include "vcpu.h"
#include "vm_mem.h"
#include "logging.h"
#include "plugin.h"

#define SYSCALL_READ    0x2000003
#define SYSCALL_WRITE   0x2000004
#define SYSCALL_OPEN    0x2000005
#define SYSCALL_CLOSE    0x2000006

int syscall_interactive = 0;

int emu_syscall_write(hv_vcpuid_t vcpu);
int emu_syscall_open(hv_vcpuid_t vcpu);
int emu_syscall_read(hv_vcpuid_t vcpu);
int emu_syscall_close(hv_vcpuid_t vcpu);

int
emu_syscall(hv_vcpuid_t vcpu)
{
    uint64_t syscall_code = rreg(vcpu, HV_X86_RAX);

    ILOG("SYSCALL %"PRIx64, syscall_code);

    switch (syscall_code) {
        case SYSCALL_WRITE:
            return emu_syscall_write(vcpu);
        case SYSCALL_OPEN:
            return emu_syscall_open(vcpu);
        case SYSCALL_CLOSE:
            return emu_syscall_close(vcpu);
        case SYSCALL_READ:
            return emu_syscall_read(vcpu);
        default:
            ELOG("unsupported syscall %llx", syscall_code);
            goto ERROR;
    }

    return 0;

ERROR:
    return -1;
}

/**
 * ssize_t write(int fildes, const void *buf, size_t nbyte);
 */
int
emu_syscall_write(hv_vcpuid_t vcpu)
{
    int fildes = rreg(vcpu, HV_X86_RDI);
    uint64_t buf_gva = rreg(vcpu, HV_X86_RSI);
    void *buf;
    size_t nbyte = rreg(vcpu, HV_X86_RDX);

    if (vm_mem_verify(buf_gva, nbyte))
    {
        ELOG("gva [%llx, %llx] out of range.", buf_gva, buf_gva + nbyte - 1);
        goto ERROR;
    }

    buf = gpa_to_hva(gva_to_gpa(buf_gva));

    if (syscall_interactive)
    {
        char input = 'q';
        input = getchar();
        if (input == 'q') {
            return 1;
        }
    }

    const struct plugin_ops *ops = get_plugin_ops();
    ssize_t ret;

    if (ops->cb_write == NULL || ops->cb_write(fildes, buf, nbyte, &ret))
    {
        ret = write(fildes, buf, nbyte);
    }

    wreg(vcpu, HV_X86_RAX, ret);

    return 0;

ERROR:
    return -1;
}

/**
 * int open(const char *path, int flags, int mode);
 */
int
emu_syscall_open(hv_vcpuid_t vcpu)
{
    uint64_t path_gva = rreg(vcpu, HV_X86_RDI);
    const char* path = gpa_to_hva(gva_to_gpa(path_gva));
    uint64_t flags = rreg(vcpu, HV_X86_RSI);
    uint64_t mode = rreg(vcpu, HV_X86_RDX);

    if (syscall_interactive)
    {
        char input = 'q';
        input = getchar();
        if (input == 'q') {
            return 1;
        }
    }

    const struct plugin_ops *ops = get_plugin_ops();
    int ret;

    if (ops->cb_open == NULL || ops->cb_open(path, flags, mode, &ret))
    {
        ret = open(path, flags, mode);
    }

    wreg(vcpu, HV_X86_RAX, ret);

    return 0;
}

/**
 * int close(int fd);
 */
int
emu_syscall_close(hv_vcpuid_t vcpu)
{
    uint64_t fd = rreg(vcpu, HV_X86_RDI);

    if (syscall_interactive)
    {
        char input = 'q';
        input = getchar();
        if (input == 'q') {
            return 1;
        }
    }

    const struct plugin_ops *ops = get_plugin_ops();
    int ret;

    if (ops->cb_close == NULL || ops->cb_close(fd, &ret))
    {
        ret = close(fd);
    }

    wreg(vcpu, HV_X86_RAX, ret);

    return 0;
}

/**
 * ssize_t read(int fildes, void *buf, size_t nbyte);
 */
int
emu_syscall_read(hv_vcpuid_t vcpu)
{
    int fildes = rreg(vcpu, HV_X86_RDI);
    uint64_t buf_gva = rreg(vcpu, HV_X86_RSI);
    void *buf;
    size_t nbyte = rreg(vcpu, HV_X86_RDX);

    if (vm_mem_verify(buf_gva, nbyte))
    {
        ELOG("gva [%llx, %llx] out of range.", buf_gva, buf_gva + nbyte - 1);
        goto ERROR;
    }

    buf = gpa_to_hva(gva_to_gpa(buf_gva));

    if (syscall_interactive)
    {
        char input = 'q';
        input = getchar();
        if (input == 'q') {
            return 1;
        }
    }

    const struct plugin_ops *ops = get_plugin_ops();
    ssize_t ret;

    if (ops->cb_read == NULL || ops->cb_read(fildes, buf, nbyte, &ret))
    {
        ret = read(fildes, buf, nbyte);
    }

    wreg(vcpu, HV_X86_RAX, ret);

    return 0;

ERROR:
    return -1;

}
