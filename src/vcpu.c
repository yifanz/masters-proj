#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "vcpu.h"
#include "vm_mem.h"
#include "emu.h"

int
vcpu_setup_ia32(hv_vcpuid_t vcpu)
{
    int err = 0;

    //TODO Assume we have the needed capabilities for now.
    //uint64_t vmx_cap_pinbased,
    //         vmx_cap_procbased,
    //         vmx_cap_procbased2,
    //         vmx_cap_entry,
    //         vmx_cap_exit;
    //
    //if (hv_vmx_read_capability(HV_VMX_CAP_PINBASED, &vmx_cap_pinbased)
    //        || hv_vmx_read_capability(HV_VMX_CAP_PROCBASED, &vmx_cap_procbased)
    //        || hv_vmx_read_capability(HV_VMX_CAP_PROCBASED2, &vmx_cap_procbased2)
    //        || hv_vmx_read_capability(HV_VMX_CAP_ENTRY, &vmx_cap_entry)
    //        || hv_vmx_read_capability(HV_VMX_CAP_EXIT, &vmx_cap_exit)
    //   )
    //{
    //    printf("read capability failed\n");
    //}

    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_CTRL_PIN_BASED, 0x000000000000003f);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_CTRL_CPU_BASED, 0x00000000b5186dfa);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_CTRL_CPU_BASED2, 0x00000000000000aa);

    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_CTRL_VMENTRY_CONTROLS, 0x00000000000091ff);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_CTRL_VMEXIT_CONTROLS, 0x0000000000236fff);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_CTRL_EXC_BITMAP, 0x0000000000040000);

    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_CTRL_CR0_MASK, 0x00000000e0000031);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_CTRL_CR0_SHADOW, 0x0000000000000021);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_CR0, 0x0000000000000031);

    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_CR3, 0x0000000000000000);

    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_CTRL_CR4_MASK, 0x0000000000002000);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_CTRL_CR4_SHADOW, 0x0000000000000000);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_CR4, 0x0000000000002000);

    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_CS, 0x0000000000000010);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_CS_BASE, 0x0000000000000000);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_CS_LIMIT, 0x00000000ffffffff);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_CS_AR, 0x000000000000c09b);

    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_DS, 0x0000000000000018);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_DS_BASE, 0x0000000000000000);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_DS_LIMIT, 0x00000000ffffffff);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_DS_AR, 0x000000000000c093);

    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_ES, 0x0000000000000018);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_ES_BASE, 0x00000000ffffffff);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_ES_LIMIT, 0x00000000ffffffff);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_ES_AR, 0x000000000000c093);

    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_FS, 0x0000000000000000);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_FS_BASE, 0x0000000000000000);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_FS_LIMIT, 0x000000000000ffff);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_FS_AR, 0x0000000000000093);

    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_GS, 0x0000000000000000);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_GS_BASE, 0x0000000000000000);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_GS_LIMIT, 0x000000000000ffff);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_GS_AR, 0x0000000000000093);

    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_SS, 0x0000000000000018);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_SS_BASE, 0x0000000000000000);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_SS_LIMIT, 0x00000000ffffffff);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_SS_AR, 0x000000000000c093);

    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_TR, 0x0000000000000000);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_TR_BASE, 0x0000000000000000);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_TR_LIMIT, 0x0000000000000000);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_TR_AR, 0x000000000000008b);

    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_LDTR, 0x0000000000000000);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_LDTR_BASE, 0x0000000000000000);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_LDTR_LIMIT, 0x000000000000ffff);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_LDTR_AR, 0x0000000000000082);

    {
        static const uint64_t gdt_gpa = 0;
        static const uint64_t null = 0x0000000000000000;
        static const uint64_t code = 0x00cf9a000000ffff;
        static const uint64_t data = 0x00cf92000000ffff;

        vm_mem_write(gdt_gpa, &null, sizeof null);
        vm_mem_write(gdt_gpa + 8, &null, sizeof null);
        vm_mem_write(gdt_gpa + 16, &code, sizeof code);
        vm_mem_write(gdt_gpa + 24, &data, sizeof data);

        hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_GDTR_BASE, gdt_gpa);
        hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_GDTR_LIMIT, 0x000000000000001f);
    }

    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_IDTR_BASE, 0x0000000000000000);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_IDTR_LIMIT, 0x000000000000ffff);

    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_IA32_EFER, 0x0000000000000000);

#define	MSR_STAR 0xc0000081 // legacy mode SYSCALL target/cs/ss
#define	MSR_LSTAR 0xc0000082 // long mode SYSCALL target rip
#define	MSR_CSTAR 0xc0000083 // compat mode SYSCALL target rip
#define	MSR_SF_MASK 0xc0000084 // syscall flags mask
#define	MSR_FSBASE 0xc0000100 // base address of the %fs "segment"
#define	MSR_GSBASE 0xc0000101 // base address of the %gs "segment"
#define	MSR_KGSBASE 0xc0000102 // base address of the kernel %gs
#define	MSR_SYSENTER_CS_MSR 0x174
#define	MSR_SYSENTER_ESP_MSR 0x175
#define	MSR_SYSENTER_EIP_MSR 0x176
#define	MSR_TSC 0x010
#define MSR_IA32_TSC_AUX 0xc0000103

    // Enable MSRs to be used directly within the guest
    if (hv_vcpu_enable_native_msr(((hv_vcpuid_t) vcpu), MSR_LSTAR, 1) ||
            hv_vcpu_enable_native_msr(((hv_vcpuid_t) vcpu), MSR_CSTAR, 1) ||
            hv_vcpu_enable_native_msr(((hv_vcpuid_t) vcpu), MSR_STAR, 1) ||
            hv_vcpu_enable_native_msr(((hv_vcpuid_t) vcpu), MSR_SF_MASK, 1) ||
            hv_vcpu_enable_native_msr(((hv_vcpuid_t) vcpu), MSR_KGSBASE, 1) ||
            hv_vcpu_enable_native_msr(vcpu, MSR_GSBASE, 1) ||
            hv_vcpu_enable_native_msr(vcpu, MSR_FSBASE, 1) ||
            hv_vcpu_enable_native_msr(vcpu, MSR_SYSENTER_CS_MSR, 1) ||
            hv_vcpu_enable_native_msr(vcpu, MSR_SYSENTER_ESP_MSR, 1) ||
            hv_vcpu_enable_native_msr(vcpu, MSR_SYSENTER_EIP_MSR, 1) ||
            hv_vcpu_enable_native_msr(vcpu, MSR_TSC, 1) ||
            hv_vcpu_enable_native_msr(vcpu, MSR_IA32_TSC_AUX, 1))
    {
        fprintf(stderr, "vcpu_setup_ia32(): MSR init failure\n");
        err = 1;
    }

    static const uint64_t start_addr = 0x0000000000008000;
    uint64_t stack_addr = 0x0000000000007000;
    uint64_t pml4_gpa = get_pml4_gpa();

    stack_addr -= 4;
    vm_mem_write(stack_addr, &pml4_gpa, 4);

    hv_vcpu_write_register(vcpu, HV_X86_RIP, start_addr);
    // RFLAGS second bit is reserved by Intel and must be set to 1
    hv_vcpu_write_register(vcpu, HV_X86_RFLAGS, 0x0000000000000002);
    hv_vcpu_write_register(vcpu, HV_X86_RAX, 0);
    hv_vcpu_write_register(vcpu, HV_X86_RCX, 0);
    hv_vcpu_write_register(vcpu, HV_X86_RDX, 0);
    hv_vcpu_write_register(vcpu, HV_X86_RBX, 0);
    hv_vcpu_write_register(vcpu, HV_X86_RSI, 0);
    hv_vcpu_write_register(vcpu, HV_X86_RDI, 0);
    hv_vcpu_write_register(vcpu, HV_X86_RSP, stack_addr);
    hv_vcpu_write_register(vcpu, HV_X86_RBP, 0);

    return err;
}

void
vcpu_dump(hv_vcpuid_t vcpu)
{
    uint64_t value = 0;

#define VMCS_DUMP_VALUE(ID) \
    hv_vmx_vcpu_read_vmcs(vcpu, ID, &value);\
	printf(#ID": 0x%llx\n", value);

    VMCS_DUMP_VALUE(VMCS_CTRL_PIN_BASED);

    VMCS_DUMP_VALUE(VMCS_CTRL_CPU_BASED);
    VMCS_DUMP_VALUE(VMCS_CTRL_CPU_BASED2);

    VMCS_DUMP_VALUE(VMCS_CTRL_VMENTRY_CONTROLS);
    VMCS_DUMP_VALUE(VMCS_CTRL_VMEXIT_CONTROLS);
    VMCS_DUMP_VALUE(VMCS_CTRL_EXC_BITMAP);

    VMCS_DUMP_VALUE(VMCS_CTRL_CR0_MASK);
    VMCS_DUMP_VALUE(VMCS_CTRL_CR0_SHADOW);
    VMCS_DUMP_VALUE(VMCS_GUEST_CR0);

    VMCS_DUMP_VALUE(VMCS_GUEST_CR3);

    VMCS_DUMP_VALUE(VMCS_CTRL_CR4_MASK);
    VMCS_DUMP_VALUE(VMCS_CTRL_CR4_SHADOW);
    VMCS_DUMP_VALUE(VMCS_GUEST_CR4);

    VMCS_DUMP_VALUE(VMCS_GUEST_CS);
    VMCS_DUMP_VALUE(VMCS_GUEST_CS_BASE);
    VMCS_DUMP_VALUE(VMCS_GUEST_CS_LIMIT);
    VMCS_DUMP_VALUE(VMCS_GUEST_CS_AR);

    VMCS_DUMP_VALUE(VMCS_GUEST_DS);
    VMCS_DUMP_VALUE(VMCS_GUEST_DS_BASE);
    VMCS_DUMP_VALUE(VMCS_GUEST_DS_LIMIT);
    VMCS_DUMP_VALUE(VMCS_GUEST_DS_AR);

    VMCS_DUMP_VALUE(VMCS_GUEST_ES);
    VMCS_DUMP_VALUE(VMCS_GUEST_ES_BASE);
    VMCS_DUMP_VALUE(VMCS_GUEST_ES_LIMIT);
    VMCS_DUMP_VALUE(VMCS_GUEST_ES_AR);

    VMCS_DUMP_VALUE(VMCS_GUEST_FS);
    VMCS_DUMP_VALUE(VMCS_GUEST_FS_BASE);
    VMCS_DUMP_VALUE(VMCS_GUEST_FS_LIMIT);
    VMCS_DUMP_VALUE(VMCS_GUEST_FS_AR);

    VMCS_DUMP_VALUE(VMCS_GUEST_GS);
    VMCS_DUMP_VALUE(VMCS_GUEST_GS_BASE);
    VMCS_DUMP_VALUE(VMCS_GUEST_GS_LIMIT);
    VMCS_DUMP_VALUE(VMCS_GUEST_GS_AR);

    VMCS_DUMP_VALUE(VMCS_GUEST_SS);
    VMCS_DUMP_VALUE(VMCS_GUEST_SS_BASE);
    VMCS_DUMP_VALUE(VMCS_GUEST_SS_LIMIT);
    VMCS_DUMP_VALUE(VMCS_GUEST_SS_AR);

    VMCS_DUMP_VALUE(VMCS_GUEST_TR);
    VMCS_DUMP_VALUE(VMCS_GUEST_TR_BASE);
    VMCS_DUMP_VALUE(VMCS_GUEST_TR_LIMIT);
    VMCS_DUMP_VALUE(VMCS_GUEST_TR_AR);

    VMCS_DUMP_VALUE(VMCS_GUEST_LDTR);
    VMCS_DUMP_VALUE(VMCS_GUEST_LDTR_BASE);
    VMCS_DUMP_VALUE(VMCS_GUEST_LDTR_LIMIT);
    VMCS_DUMP_VALUE(VMCS_GUEST_LDTR_AR);

    VMCS_DUMP_VALUE(VMCS_GUEST_GDTR_BASE);
    VMCS_DUMP_VALUE(VMCS_GUEST_GDTR_LIMIT);

    VMCS_DUMP_VALUE(VMCS_GUEST_IDTR_BASE);
    VMCS_DUMP_VALUE(VMCS_GUEST_IDTR_LIMIT);

    VMCS_DUMP_VALUE(VMCS_GUEST_IA32_EFER);

#undef VMCS_DUMP_VALUE

#define VCPU_DUMP_VALUE(ID) \
    hv_vcpu_read_register(vcpu, ID, &value);\
	printf(#ID": 0x%llx\n", value);

    VCPU_DUMP_VALUE(HV_X86_RIP);
    VCPU_DUMP_VALUE(HV_X86_RFLAGS);
    VCPU_DUMP_VALUE(HV_X86_RAX);
    VCPU_DUMP_VALUE(HV_X86_RCX);
    VCPU_DUMP_VALUE(HV_X86_RDX);
    VCPU_DUMP_VALUE(HV_X86_RBX);
    VCPU_DUMP_VALUE(HV_X86_RSI);
    VCPU_DUMP_VALUE(HV_X86_RDI);
    VCPU_DUMP_VALUE(HV_X86_RSP);
    VCPU_DUMP_VALUE(HV_X86_RBP);

#undef VCPU_DUMP_VALUE
}

void vcpu_run(hv_vcpuid_t vcpu)
{
    int stop = 0;
    do {
        hv_return_t error = hv_vcpu_run(vcpu);
        if (error)
        {
            fprintf(stderr, "hv_vcpu_run error: 0x%x\n", error);
            break;
        }

        uint64_t exit_reason = 0;
        hv_vmx_vcpu_read_vmcs(vcpu, VMCS_RO_EXIT_REASON, &exit_reason);
        uint64_t exit_qualification = 0;
        hv_vmx_vcpu_read_vmcs(vcpu, VMCS_RO_EXIT_QUALIFIC,
                &exit_qualification);
        uint64_t exit_instr_len = 0;
        hv_vmx_vcpu_read_vmcs(vcpu, VMCS_RO_VMEXIT_INSTR_LEN,
                &exit_instr_len);

        printf("--VMEXIT-----------------------------\n");

        printf("VMCS_RO_EXIT_REASON: 0x%llx\n"
                "VMCS_RO_EXIT_QUALIFIC: 0x%llx\n"
                "VMCS_RO_VMEXIT_INSTR_LEN: 0x%llx\n", 
                exit_reason,
                exit_qualification,
                exit_instr_len);
        vcpu_dump(vcpu);

        switch (exit_reason) {
            case VMX_REASON_EXC_NMI:
                {
                    printf("VMX_REASON_EXC_NMI\n");
                    uint64_t vecinfo = 0;
                    hv_vmx_vcpu_read_vmcs(vcpu,
                            VMCS_RO_IDT_VECTOR_INFO, &vecinfo);
                    uint8_t interrupt_number = vecinfo & 0xFF;
                    printf("interrupt number %u\n", interrupt_number);
                    if (interrupt_number == 0) {
                        printf("Maybe general protection exception?\n");
                        stop = 1;
                        break;
                    }
                    hv_vcpu_write_register(vcpu, HV_X86_RIP,
                            rreg(vcpu, HV_X86_RIP) + 2);
                    break;
                }
            case VMX_REASON_IRQ:
                // Probably time-slice was up
                printf("VMX_REASON_IRQ\n");

                break;
            case VMX_REASON_VMCALL:
                printf("VMX_REASON_VMCALL\n");
                if (rreg(vcpu, HV_X86_RIP) == 0x7fff5fc22bee) {
                    printf("skipping spinlock\n");
                    hv_vcpu_write_register(vcpu, HV_X86_RIP, 0x7fff5fc22c71);
                } else {
                    hv_vcpu_write_register(vcpu, HV_X86_RIP,
                            rreg(vcpu, HV_X86_RIP)
                            + exit_instr_len);
                }
                break;
            case VMX_REASON_HLT:
                printf("VMX_REASON_HLT\n");
                stop = 1;
                break;
            case VMX_REASON_EPT_VIOLATION:
                // handle memory access here
                printf("VMX_REASON_EPT_VIOLATION\n");
                break;
            case VMX_REASON_RDMSR:
                printf("VMX_REASON_RDMSR\n");
                stop = emu_rdmsr(vcpu);
                break;
            case VMX_REASON_WRMSR:
                printf("VMX_REASON_WRMSR\n");
                stop = emu_wrmsr(vcpu);
                break;
            case VMX_REASON_MOV_CR:
                printf("VMX_REASON_MOV_CR\n");
                stop = emu_mov_cr(vcpu);
                break;
            case VMX_REASON_CPUID:
                printf("CPUID\n");
                stop = emu_cpuid(vcpu);
                break;
            default:
                {
                    // TODO find better way to do this
                    // figure out if syscall was made. 
                    // syscall saves addr of next instruction in rcx
                    uint64_t rcx = 0;
                    hv_vcpu_read_register(vcpu, HV_X86_RCX, &rcx);
                    if (rcx >= 2) {
                        // syscall instr length 2 bytes
                        uint64_t gpa = gva_to_gpa(rcx - 2);
                        //printf("%llx\n", gpa);
                        uint16_t op = 0;
                        if (vm_mem_read(gpa, &op, sizeof op) == sizeof op
                                && op == 0x050f) {
                            printf("SYSCALL\n");
                            char input = 'q';
                            input = getchar();
                            if (input == 'q') {
                                stop = 1;
                                break;
                            }
                            hv_vcpu_write_register(vcpu, HV_X86_RIP, rcx);

                            // do corresponding syscall on host
                            uint64_t syscall_code = rreg(vcpu, HV_X86_RAX);
                            if (syscall_code == 0x2000004)
                            {
                                int fd = rreg(vcpu, HV_X86_RDI);
                                void *buf = gpa_to_hva(
                                        gva_to_gpa(rreg(vcpu, HV_X86_RSI))
                                        );
                                size_t count = rreg(vcpu, HV_X86_RDX);
                                ssize_t ret = write(fd, buf, count);
                                wreg(vcpu, HV_X86_RAX, ret);
                            }
                            else
                            {
                                printf("Unhandled syscall\n"); 
                                stop = 1;
                            }
                            break;
                        }
                    }
                    printf("Unhandled exit\n");
                    stop = 1;
                }
        }
    } while (!stop);
}

uint64_t
rreg(hv_vcpuid_t vcpu, hv_x86_reg_t reg)
{
	uint64_t val = 0;

	if (hv_vcpu_read_register(vcpu, reg, &val))
    {
        fprintf(stderr, "rreg(): hv_vcpu_read_register failed\n");
	}

	return val;
}

void
wreg(hv_vcpuid_t vcpu, hv_x86_reg_t reg, uint64_t val)
{
    if (hv_vcpu_write_register(vcpu, reg, val))
    {
        fprintf(stderr, "wreg(): hv_vcpu_write_register failed\n");
    }
}
