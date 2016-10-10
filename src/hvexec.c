#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdint.h>
#include <unistd.h>
#include <Hypervisor/hv.h>
#include <Hypervisor/hv_vmx.h>

#include "global.h"
#include "mem.h"
#include "reg.h"
#include "dump.h"
#include "emu.h"

void *vm_mem;

int
main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: hvexec boot basic.o\n");
        exit(1);
    }

	if (hv_vm_create(HV_VM_DEFAULT))
    {
		abort();
	}

	uint64_t vmx_cap_pinbased,
             vmx_cap_procbased,
             vmx_cap_procbased2,
             vmx_cap_entry,
             vmx_cap_exit;

    if (hv_vmx_read_capability(HV_VMX_CAP_PINBASED, &vmx_cap_pinbased)
            || hv_vmx_read_capability(HV_VMX_CAP_PROCBASED, &vmx_cap_procbased)
            || hv_vmx_read_capability(HV_VMX_CAP_PROCBASED2, &vmx_cap_procbased2)
            || hv_vmx_read_capability(HV_VMX_CAP_ENTRY, &vmx_cap_entry)
            || hv_vmx_read_capability(HV_VMX_CAP_EXIT, &vmx_cap_exit)
       )
    {
        printf("read capability failed\n");
	}

	if (!(vm_mem = valloc(VM_MEM_SIZE)))
    {
		abort();
	}

	if (hv_vm_map(vm_mem, 0, VM_MEM_SIZE, 
                HV_MEMORY_READ | HV_MEMORY_WRITE | HV_MEMORY_EXEC))
	{
		abort();
	}

#define BASE_GDT 0x0000ull
	uint64_t *gdt_entry;
    gdt_entry = ((uint64_t *) ((uintptr_t) vm_mem + BASE_GDT));
	gdt_entry[0] = 0x0000000000000000; // null
	gdt_entry[1] = 0x0000000000000000; // null
	gdt_entry[2] = 0x00cf9a000000ffff; // code
	gdt_entry[3] = 0x00cf92000000ffff; // data

	hv_vcpuid_t vcpu;

	if (hv_vcpu_create(&vcpu, HV_VCPU_DEFAULT))
    {
		abort();
	}

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

    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_GDTR_BASE, BASE_GDT);
    hv_vmx_vcpu_write_vmcs(vcpu, VMCS_GUEST_GDTR_LIMIT, 0x000000000000001f);

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
        fprintf(stderr, "MSR init failure\n");
        abort();
    }

#define START_ADDR 0x0000000000006000
    hv_vcpu_write_register(vcpu, HV_X86_RIP, START_ADDR);
    // RFLAGS second bit is reserved by Intel and must be set to 1
    hv_vcpu_write_register(vcpu, HV_X86_RFLAGS, 0x0000000000000002);
    hv_vcpu_write_register(vcpu, HV_X86_RAX, 0);
    hv_vcpu_write_register(vcpu, HV_X86_RCX, 0);
    hv_vcpu_write_register(vcpu, HV_X86_RDX, 0);
    hv_vcpu_write_register(vcpu, HV_X86_RBX, 0);
    hv_vcpu_write_register(vcpu, HV_X86_RSI, 0);
    hv_vcpu_write_register(vcpu, HV_X86_RDI, 0);
    hv_vcpu_write_register(vcpu, HV_X86_RSP, 0);
    hv_vcpu_write_register(vcpu, HV_X86_RBP, 0);

    //uint64_t boot_size = 0;
    {
        FILE *f = fopen(argv[1], "r");
        struct stat st;
        stat(argv[1], &st);
        //boot_size = st.st_size;
        fread((void *) ((uintptr_t) vm_mem + START_ADDR), 1, st.st_size, f);
        fclose(f);
    }

    {
#define START_ADDR_EXE 0x0000000000200000
        FILE *f = fopen(argv[2], "r");
        struct stat st;
        stat(argv[2], &st);
        fread((void *) ((uintptr_t) vm_mem + START_ADDR_EXE), 1, st.st_size, f);
        fclose(f);
    }

    printf("Ready to launch\n");
    vcpu_dump(vcpu);

	int stop = 0;
	do {
        hv_return_t error = hv_vcpu_run(vcpu);
		if (error)
        {
            fprintf(stderr, "hv_vcpu_run error: 0x%x\n", error);
			abort();
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
                hv_vcpu_write_register(vcpu, HV_X86_RIP,
                        rreg(vcpu, HV_X86_RIP)
                        + exit_instr_len);
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
                        int err = 0;
                        uint64_t gpa = gva_to_gpa(rcx - 2);
                        //printf("%llx\n", gpa);
                        uint16_t op = rmem16(gpa, &err);
                        if (!err && op == 0x050f) {
                            printf("SYSCALL\n");
                            hv_vcpu_write_register(vcpu, HV_X86_RIP, rcx);

                            // do corresponding syscall on host
                            uint64_t syscall_code = rreg(vcpu, HV_X86_RAX);
                            if (syscall_code == 0x2000004)
                            {
                                int fd = rreg(vcpu, HV_X86_RDI);
                                uintptr_t buf = (uintptr_t) vm_mem 
                                    + gva_to_gpa(rreg(vcpu, HV_X86_RSI));
                                size_t count = rreg(vcpu, HV_X86_RDX);
                                ssize_t ret = write(fd, (void*) buf, count);
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

	if (hv_vcpu_destroy(vcpu))
    {
		abort();
	}

	if (hv_vm_unmap(0, VM_MEM_SIZE))
    {
		abort();
	}

	if (hv_vm_destroy())
    {
		abort();
	}

	free(vm_mem);

	return 0;
}
