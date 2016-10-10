#ifndef emu_h
#define emu_h

int emu_rdmsr(int vcpu);
int emu_wrmsr(int vcpu);

int emu_cpuid(int vcpu);
int emu_mov_cr(int vcpu);

#endif /* emu_h */
