/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */
#ifndef vcpu_h
#define vcpu_h

#include <stdint.h>
#include <Hypervisor/hv.h>
#include <Hypervisor/hv_vmx.h>

int vcpu_setup_ia32(hv_vcpuid_t vcpu);
void vcpu_dump(hv_vcpuid_t vcpu);
void vcpu_run(hv_vcpuid_t vcpu);

uint64_t rreg(hv_vcpuid_t vcpu, hv_x86_reg_t reg);
void wreg(hv_vcpuid_t vcpu, hv_x86_reg_t reg, uint64_t val);

int push34(hv_vcpuid_t vcpu, uint32_t val);
int push64(hv_vcpuid_t vcpu, uint64_t val);

#endif /* vcpu_h */
