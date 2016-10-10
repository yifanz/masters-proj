#include <stdio.h>
#include <Hypervisor/hv.h>
#include <Hypervisor/hv_vmx.h>

#include "dump.h"

void
vcpu_dump(int vcpuid)
{
    uint64_t value = 0;

#define VMCS_DUMP_VALUE(ID) \
    hv_vmx_vcpu_read_vmcs(vcpuid, ID, &value);\
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
    hv_vcpu_read_register(vcpuid, ID, &value);\
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

