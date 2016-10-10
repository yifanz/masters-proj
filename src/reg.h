#ifndef reg_h
#define reg_h

#include <Hypervisor/hv.h>
#include <Hypervisor/hv_vmx.h>
#include <stdint.h>

uint64_t rreg(hv_vcpuid_t vcpu, hv_x86_reg_t reg);
void wreg(hv_vcpuid_t vcpu, hv_x86_reg_t reg, uint64_t val);

#endif /* reg_h */
