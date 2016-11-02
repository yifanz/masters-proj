#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <Hypervisor/hv.h>
#include <Hypervisor/hv_vmx.h>

#include "mem.h"

#define VM_MEM_SIZE (500 * 1024 * 1024)
#define PML4E_ADDR 0x1000

void *vm_mem;

int
vm_mem_init()
{
    if (!(vm_mem = valloc(VM_MEM_SIZE)))
    {
        fprintf(stderr, "vm_mem_init(): valloc failed\n");
        return 1;
    }

    if (hv_vm_map(vm_mem, 0, VM_MEM_SIZE, 
                HV_MEMORY_READ | HV_MEMORY_WRITE | HV_MEMORY_EXEC))
	{
        fprintf(stderr, "vm_mem_init(): hv_vm_map failed\n");
        return 1;
	}
    return 0;
}

void
vm_mem_destroy()
{
    if (hv_vm_unmap(0, VM_MEM_SIZE))
    {
		fprintf(stderr, "vm_mem_destroy(): hv_vm_unmap failed\n");
	}

    free(vm_mem);
}

int
rmem(uint64_t offset, void* buf, int bytes)
{
    uint64_t mem = (uint64_t) vm_mem;
    uint64_t max_byte = mem + VM_MEM_SIZE - 1;
    uint64_t addr = mem + offset;
    uint64_t last_byte = addr + bytes - 1;
    uint64_t out = (uint64_t) buf;
    int bytes_read = 0;

    if (addr > max_byte)
    {
        fprintf(stderr, "rmem(): addr (0x%llx) out of bounds\n", addr);
        return 0;
    }

    if (last_byte > max_byte)
    {
        last_byte = max_byte;
    }

    while (addr <= last_byte)
    {
        *((uint8_t *) out++) = *((uint8_t *) addr++);
        bytes_read++;
    }

    return bytes_read;
}

uint8_t
rmem8(uint64_t offset)
{
    uint8_t out = 0;
    rmem(offset, &out, sizeof out);

    return out;
}

uint16_t
rmem16(uint64_t offset)
{
    uint16_t out = 0;
    rmem(offset, &out, sizeof out);

    return out;
}

uint32_t
rmem32(uint64_t offset)
{
    uint32_t out = 0;
    rmem(offset, &out, sizeof out);

    return out;
}

uint64_t
rmem64(uint64_t offset)
{
    uint64_t out = 0;
    rmem(offset, &out, sizeof out);

    return out;
}

int
wmem(uint64_t offset, void *buf, int bytes)
{
    uint64_t mem = (uint64_t) vm_mem;
    uint64_t max_byte = mem + VM_MEM_SIZE - 1;
    uint64_t addr = mem + offset;
    uint64_t last_byte = addr + bytes - 1;
    uint64_t in = (uint64_t) buf;
    int bytes_written = 0;

    if (addr > max_byte)
    {
        fprintf(stderr, "wmem(): addr (0x%llx) out of bounds\n", addr);
        return 0;
    }

    if (last_byte > max_byte)
    {
        last_byte = max_byte;
    }

    while (addr <= last_byte)
    {
        *((uint8_t *) addr++) = *((uint8_t *) in++);
        bytes_written++;
    }

    return bytes_written;
}

int
wmem8(uint64_t offset, uint8_t in)
{
    return wmem(offset, &in, sizeof in);
}

int
wmem16(uint64_t offset, uint16_t in)
{
    return wmem(offset, &in, sizeof in);
}

int
wmem32(uint64_t offset, uint32_t in)
{
    return wmem(offset, &in, sizeof in);
}

int
wmem64(uint64_t offset, uint64_t in)
{
    return wmem(offset, &in, sizeof in);
}

uint64_t
gva_to_gpa(uint64_t gva)
{
    uint64_t mem = (uint64_t) vm_mem;

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

void* vm_mem_ptr(uint64_t offset)
{
    uint64_t mem = (uint64_t) vm_mem;
    uint64_t max_byte = mem + VM_MEM_SIZE - 1;
    uint64_t addr = mem + offset;

    if (addr > max_byte)
    {
        fprintf(stderr, "vm_mem_ptr(): addr (0x%llx) out of bounds\n", addr);
        return NULL;
    }

    return (void *) addr;
}
