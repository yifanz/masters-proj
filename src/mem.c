#include <stdio.h>

#include "global.h"
#include "mem.h"

#define PML4E_ADDR 0x1000

uint16_t
rmem16(uintptr_t offset, int* err)
{
    uintptr_t max = (uintptr_t) vm_mem + VM_MEM_SIZE - 1;
    uintptr_t addr = (uintptr_t) vm_mem + offset;

    if (addr > max)
    {
        *err = 1;
        return 0;
    }

    uintptr_t diff = max - addr;
    if (diff < 1)
    {
        return (*((uint16_t *) max - 1)) >> diff;
    }

    return *((uint16_t *) addr);
}

uint64_t
gva_to_gpa(uint64_t gva)
{
    uintptr_t mem = (uintptr_t) vm_mem;

    uint64_t offset = gva & 0x1FFFFF;
    uint64_t PDE_idx = (gva >> 21) & 0x1FF;
    uint64_t PDPTE_idx = (gva >> 30) & 0x1FF;
    uint64_t PML4E_idx = (gva >> 39) & 0x1FF;

    // PML4E
    uint64_t entry = *((uint64_t*) (mem + PML4E_ADDR + PML4E_idx * 8));
    uint64_t addr = entry & 0xFFFFFFFFFF000;
    //printf("PDPTE addr %llx\n", addr);
    // PDPTE
    entry = *((uint64_t*) (mem + addr + PDPTE_idx * 8));
    addr = entry & 0xFFFFFFFFFF000;
    //printf("PDE addr %llx\n", addr);
    // PDE
    entry = *((uint64_t*) (mem + addr + PDE_idx * 8));
    addr = entry & 0xFFFFFFFE00000;

    addr = addr | offset;

    return addr;
}
