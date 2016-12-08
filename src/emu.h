/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */
#ifndef emu_h
#define emu_h

#include <Hypervisor/hv.h>
#include <Hypervisor/hv_vmx.h>

int emu_rdmsr(hv_vcpuid_t vcpu);
int emu_wrmsr(hv_vcpuid_t vcpu);

int emu_cpuid(hv_vcpuid_t vcpu);
int emu_mov_cr(hv_vcpuid_t vcpu);

#endif /* emu_h */
