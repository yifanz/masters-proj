#include <stdlib.h>

#include "reg.h"

uint64_t
rreg(hv_vcpuid_t vcpu, hv_x86_reg_t reg)
{
	uint64_t v;

	if (hv_vcpu_read_register(vcpu, reg, &v))
    {
		abort();
	}

	return v;
}

void
wreg(hv_vcpuid_t vcpu, hv_x86_reg_t reg, uint64_t pval)
{
    if (hv_vcpu_write_register(vcpu, reg, pval))
    {
        abort();
    }
}
