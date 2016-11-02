#include <stdio.h>
#include <stdlib.h>

#include "emu.h"
#include "vcpu.h"

static uint64_t vmx_get_guest_reg(hv_vcpuid_t vcpu, int ident)
{
    switch (ident) {
    case 0:
        return (rreg(vcpu, HV_X86_RAX));
    case 1:
        return (rreg(vcpu, HV_X86_RCX));
    case 2:
        return (rreg(vcpu, HV_X86_RDX));
    case 3:
        return (rreg(vcpu, HV_X86_RBX));
    case 4:
        return (rreg(vcpu, HV_X86_RSP));
    case 5:
        return (rreg(vcpu, HV_X86_RBP));
    case 6:
        return (rreg(vcpu, HV_X86_RSI));
    case 7:
        return (rreg(vcpu, HV_X86_RDI));
    case 8:
        return (rreg(vcpu, HV_X86_R8));
    case 9:
        return (rreg(vcpu, HV_X86_R9));
    case 10:
        return (rreg(vcpu, HV_X86_R10));
    case 11:
        return (rreg(vcpu, HV_X86_R11));
    case 12:
        return (rreg(vcpu, HV_X86_R12));
    case 13:
        return (rreg(vcpu, HV_X86_R13));
    case 14:
        return (rreg(vcpu, HV_X86_R14));
    case 15:
        return (rreg(vcpu, HV_X86_R15));
    default:
        printf("invalid vmx register %d", ident);
        abort();
    }
}

int
emu_rdmsr(hv_vcpuid_t vcpu)
{
    int stop = 0;
    uint32_t ecx =
        (uint32_t) rreg(vcpu, HV_X86_RCX);

    if (ecx == 0xc0000080) {
        printf("RDMSR EFER\n");
        uint64_t efer = 0;
        hv_vmx_vcpu_read_vmcs(vcpu,
                VMCS_GUEST_IA32_EFER, &efer);
        hv_vcpu_write_register(vcpu, HV_X86_RAX,
                (uint32_t) efer); 
        hv_vcpu_write_register(vcpu, HV_X86_RDX,
                (uint32_t) (efer >> 32)); 
        hv_vcpu_write_register(vcpu, HV_X86_RIP,
                rreg(vcpu, HV_X86_RIP)
                + 2);
    } else {
        printf("Unsupported MSR\n");
        stop = 1;
    }

    return stop;
}

int
emu_wrmsr(hv_vcpuid_t vcpu)
{
    int stop = 0;
    uint64_t tmp = 0;
    uint32_t eax = 0;
    hv_vcpu_read_register(vcpu, HV_X86_RAX, &tmp);
    eax = (uint32_t) tmp;
    uint32_t ecx = 0;
    hv_vcpu_read_register(vcpu, HV_X86_RCX, &tmp);
    ecx = (uint32_t) tmp;
    uint32_t edx = 0;
    hv_vcpu_read_register(vcpu, HV_X86_RDX, &tmp);
    edx = (uint32_t) tmp;

    if (ecx == 0xc0000080) {
        printf("WRMSR EFER\n");
        hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_IA32_EFER,
                (uint64_t) edx << 32 | eax);
        hv_vcpu_write_register(vcpu, HV_X86_RIP,
                rreg(vcpu, HV_X86_RIP)
                + 2);
    } else {
        printf("Unsupported MSR ecx=0x%u\n", ecx);
        stop = 1;
    }

    return stop;
}

int
emu_mov_cr(hv_vcpuid_t vcpu)
{
    int stop = 0;
    uint64_t exit_qualification = 0;
    hv_vmx_vcpu_read_vmcs(vcpu, VMCS_RO_EXIT_QUALIFIC,
            &exit_qualification);

    switch (exit_qualification & 0xf) {
        case 0:
            printf("emulate cr0\n");
            if ((exit_qualification & 0xf0) != 0x00) {
                printf("Only MOV to cr0 supported\n");
                break;
            }
            uint64_t regval = 
                vmx_get_guest_reg(vcpu,
                        (exit_qualification >> 8) & 0xf);
            hv_vmx_vcpu_write_vmcs(vcpu,
                    VMCS_CTRL_CR0_SHADOW,
                    regval);
            hv_vcpu_write_register(vcpu, HV_X86_CR0,
                    regval);
            if (regval & 0x80000000) {
                uint64_t efer, entryctls;
                hv_vmx_vcpu_read_vmcs(vcpu,
                        VMCS_GUEST_IA32_EFER, &efer);
#define EFER_SCE 0x000000001
#define EFER_LME 0x000000100
#define EFER_LMA 0x000000400
#define EFER_NXE 0x000000800
#define VM_ENTRY_GUEST_LMA (1u << 9)
                if (efer & EFER_LME) {
                    efer |= EFER_LMA | EFER_NXE | EFER_SCE;
                    hv_vmx_vcpu_write_vmcs(vcpu,
                            VMCS_GUEST_IA32_EFER, efer);
                    hv_vmx_vcpu_read_vmcs(vcpu,
                            VMCS_CTRL_VMENTRY_CONTROLS,
                            &entryctls
                            );
                    entryctls |= VM_ENTRY_GUEST_LMA;
                    hv_vmx_vcpu_write_vmcs(vcpu,
                            VMCS_CTRL_VMENTRY_CONTROLS,
                            entryctls);
                }
            }
            hv_vcpu_write_register(vcpu, HV_X86_RIP,
                    rreg(vcpu, HV_X86_RIP)
                    + 3);
            break;
        case 4:
            printf("emulate cr4\n");
            break;
        case 8:
            printf("emulate cr8\n");
            break;
    }

    return stop;
}

int
emu_cpuid(hv_vcpuid_t vcpu)
{
    int stop = 0;
    // Emulate CPUID to show long mode capable processor
    uint64_t rax = rreg(vcpu, HV_X86_RAX);
    if (rax == 0x80000000) {
        wreg(vcpu, HV_X86_RAX, 0x80000001);
    } else if (rax == 0x80000001) {
        wreg(vcpu, HV_X86_RDX, (1 << 29));    
    }
    wreg(vcpu, HV_X86_RIP, rreg(vcpu, HV_X86_RIP) + 2);

    return stop;
}
