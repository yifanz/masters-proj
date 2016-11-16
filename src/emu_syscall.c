#include <Hypervisor/hv.h>
#include <Hypervisor/hv_vmx.h>
#include <unistd.h>

#include "emu_syscall.h"
#include "vcpu.h"
#include "vm_mem.h"
#include "logging.h"

#define SYSCALL_WRITE 0x2000004

int emu_syscall_write(hv_vcpuid_t vcpu);

int
emu_syscall(hv_vcpuid_t vcpu)
{
    uint64_t syscall_code = rreg(vcpu, HV_X86_RAX);

    switch (syscall_code) {
        case SYSCALL_WRITE:
            return emu_syscall_write(vcpu);
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

    ssize_t ret = write(fildes, buf, nbyte);

    wreg(vcpu, HV_X86_RAX, ret);

    return 0;

ERROR:
    return -1;
}
