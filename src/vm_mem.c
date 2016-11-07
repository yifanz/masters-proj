#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <inttypes.h>

#include "vm_mem.h"

#define PAGE_SIZE 0x1000

// Translation from IA32e linear to physical address using 4KB paging.
// 48 out of 64 bits are used use for addressing, the other bits are metadata.
struct virtual_address
{
    // Calculate page structure index by multiplying 9 bits from the virt addr by 8
    // to align the index with 8 byte page structure entries.
    // See decode_gva() function.
    uint16_t pml4_idx;
    uint16_t pdpt_idx;
    uint16_t pd_idx;
    uint16_t pt_idx;
    // 12 bits for byte offset into 4KB page
    uint16_t offset;
};

// Mapping of a region of memory in the guest physical address space to the host
// virtual address space.
struct mem_region
{
    uint64_t gpa;
    size_t npages;
    void *hva;
    struct mem_region *next;
};

int find_free_mem_region(uint64_t *gpa, size_t npages);
void decode_gva(uint64_t gva, struct virtual_address *vaddr);

// Tracks all regions of memory mapped between guest physical and host virtual
// address spaces.
struct mem_region *mem_regions = NULL;

// Root level paging structure
void *pml4 = NULL;
uint64_t pml4_gpa;

int
find_free_mem_region(uint64_t *gpa, size_t npages)
{
    if (npages < 1) return -1;

    struct mem_region *p = mem_regions;
    uint64_t start = 0;
    uint64_t end;

    while(p != NULL)
    {
        end = p->gpa;

        if (end - start >= npages * PAGE_SIZE) break;

        start = p->gpa + p->npages * PAGE_SIZE;
        p = p->next;
    }

    *gpa = start;
    return 0;
}

void
decode_gva(uint64_t gva, struct virtual_address *vaddr)
{
    // First 12 bits represent offset in a 4KB page
    vaddr->offset = gva & 0xFFF;

    // Each paging structure has 512 entries.
    // Thus, 9 bits are needed for each index.

    // PT holds the page aligned physical address of 4KB page.
    vaddr->pt_idx = ((gva >> 12) & 0x1FF) * 8; // page table
    vaddr->pd_idx = ((gva >> 21) & 0x1FF) * 8; // page directory
    vaddr->pdpt_idx = ((gva >> 30) & 0x1FF) * 8; // page directory pointer
    vaddr->pml4_idx = ((gva >> 39) & 0x1FF) * 8; // page map level 4
}

void
vm_mem_dump()
{
    struct mem_region *p = mem_regions;
    uint64_t start = 0;
    uint64_t end;

    while (p != NULL)
    {
        end = p->gpa;

        if (start < end)
        {
            printf("[0x%016"PRIx64", 0x%016"PRIx64"] %"PRIu64" pages free\n",
                    start,
                    end - 1,
                    (end - start) / PAGE_SIZE);
        }

        start = end;
        end = p->gpa + p->npages * PAGE_SIZE;

        printf("[0x%016"PRIx64", 0x%016"PRIx64"] %"PRIu64" pages used\n",
                start,
                end - 1,
                (end - start) / PAGE_SIZE);

        start = end;

        p = p->next;
    }
}

void*
vm_mem_alloc(size_t npages, uint64_t *gpa)
{
    void *hva = NULL;
    uint64_t free_gpa;

    if (find_free_mem_region(&free_gpa, npages))
    {
        fprintf(stderr, "vm_mem_alloc(): cannot find free memory region.\n");
        goto ERROR;
    }

    hva = valloc(npages * PAGE_SIZE);

    if (hva == NULL)
    {
        fprintf(stderr, "vm_mem_alloc(): valloc failed.\n");
        goto ERROR;
    }

    if (map_gpa(free_gpa, npages, hva))
    {
        fprintf(stderr, "vm_mem_alloc(): failed to map gpa.\n");
        goto ERROR;
    }

    if (gpa != NULL) *gpa = free_gpa;

    return hva;

ERROR:
    free(hva);
    return NULL;
}

int
map_gva(uint64_t gva, size_t npages, uint64_t gpa)
{
    static const uint64_t PRESENT = 0x1; // bit 0
    static const uint64_t WRITE = 0x2; // bit 1

    struct virtual_address vaddr;

    // Addresses must be aligned at page boundary.
    gva &= 0xFFFFFFFFFFFFF000;
    gpa &= 0xFFFFFFFFFFFFF000;

    if (pml4 == NULL)
    {
        pml4 = vm_mem_alloc(1, &pml4_gpa);

        if (pml4 == NULL)
        {
            fprintf(stderr, "map_gva(): vm_mem_alloc failed.\n");
            goto ERROR;
        }
    }

    while(npages--) {
        decode_gva(gva, &vaddr);
 
        uint64_t pml4e;
        uint64_t pdpt_gpa;
        uint64_t pdpte;
        uint64_t pd_gpa;
        uint64_t pde;
        uint64_t pt_gpa;
        uint64_t pte;

        vm_mem_read(pml4_gpa + vaddr.pml4_idx, &pml4e, sizeof pml4e);

        if (pml4e == 0)
        {
            vm_mem_alloc(1, &pdpt_gpa);
        }
        else
        {
            pdpt_gpa = pml4e & 0xFFFFFFFFFFFFF000;
        }

        pml4e = 0 | PRESENT | WRITE | pdpt_gpa;
        vm_mem_write(pml4_gpa + vaddr.pml4_idx, &pml4e, sizeof pml4e);

        vm_mem_read(pdpt_gpa + vaddr.pdpt_idx, &pdpte, sizeof pdpte);

        if (pdpte == 0)
        {
            vm_mem_alloc(1, &pd_gpa);
        }
        else
        {
            pd_gpa = pml4e & 0xFFFFFFFFFFFFF000;
        }

        pdpte = 0 | PRESENT | WRITE | pd_gpa;
        vm_mem_write(pdpt_gpa + vaddr.pdpt_idx, &pdpte, sizeof pdpte);

        vm_mem_read(pd_gpa + vaddr.pd_idx, &pde, sizeof pde);

        if (pde == 0)
        {
            vm_mem_alloc(1, &pt_gpa);
        }
        else
        {
            pt_gpa = pml4e & 0xFFFFFFFFFFFFF000;
        }

        pde = 0 | PRESENT | WRITE | pt_gpa;
        vm_mem_write(pd_gpa + vaddr.pd_idx, &pde, sizeof pde);

        pte = 0 | PRESENT | WRITE | (gpa & 0xFFFFFFFFFFFFF000);
        vm_mem_write(pt_gpa + vaddr.pt_idx, &pte, sizeof pte);

        gva += PAGE_SIZE;
        gpa += PAGE_SIZE;
    }

    return 0;
ERROR:
    return -1;
}

int
unmap_gva(uint64_t gva, size_t npages)
{
    // TODO
    return -1;
}

int
map_gpa(uint64_t gpa, size_t npages, void* hva)
{
    struct mem_region **p = &mem_regions;
    struct mem_region *next;

    if (npages < 1) goto ERROR;

    // Address must be aligned to page boundary.
    gpa &= 0xFFFFFFFFFFFFF000;

    while(*p != NULL && (*p)->gpa + (*p)->npages * PAGE_SIZE <= gpa)
        p = &(*p)->next;

    if (*p == NULL)
    {
        next = NULL; 
    }
    else if ((*p)->gpa >= gpa + npages * PAGE_SIZE)
    {
        next = (*p)->next;
    }
    else
    {
        fprintf(stderr, "map_gpa(): 0x%"PRIx64" overlaps existing region "
                "[0x%"PRIx64" 0x%"PRIx64"]\n",
                gpa, (*p)->gpa, (*p)->gpa + (*p)->npages * PAGE_SIZE - 1);
        goto ERROR;
    }

    if (hv_vm_map(hva, gpa, npages * PAGE_SIZE, 
                HV_MEMORY_READ | HV_MEMORY_WRITE | HV_MEMORY_EXEC))
    {
        fprintf(stderr, "map_gpa(): hv_vm_map failed\n");
        goto ERROR;
    }

    *p = (struct mem_region*) malloc(sizeof(struct mem_region));

    if (*p == NULL)
    {
        fprintf(stderr, "map_gpa(): malloc failed\n");
        *p = next;
        goto ERROR_MALLOC;
    }

    **p = (struct mem_region) { 
        .gpa = gpa, .npages = npages, .hva = hva, .next = next
    };

    return 0;

ERROR_MALLOC:
    hv_vm_unmap(gpa, npages * PAGE_SIZE);
ERROR:
    return -1;
}

int
unmap_gpa(uint64_t gpa, size_t npages)
{
    // TODO
    return -1;
}

uint64_t
gva_to_gpa(uint64_t gva)
{
    uint64_t gpa = 0;
    struct virtual_address vaddr = {0};
    uint64_t entry;

    decode_gva(gva, &vaddr);

    vm_mem_read(pml4_gpa + vaddr.pml4_idx, &entry, sizeof entry); // pml4
    gpa = entry & 0xFFFFFFFFFF000;
    vm_mem_read(gpa + vaddr.pdpt_idx, &entry, sizeof entry); // pdpt
    gpa = entry & 0xFFFFFFFFFF000;
    vm_mem_read(gpa + vaddr.pd_idx, &entry, sizeof entry); // pd
    gpa = entry & 0xFFFFFFFFFF000;
    vm_mem_read(gpa + vaddr.pt_idx, &entry, sizeof entry); // pt
    gpa = entry & 0xFFFFFFFFFF000;

    // offset
    gpa |= vaddr.offset;

    return gpa;
}

void*
gpa_to_hva(uint64_t gpa)
{
    struct mem_region *p = mem_regions;
    void *hva = NULL;
    
    while(p != NULL && p->gpa <= gpa)
    {
        if (gpa < p->gpa + p->npages * PAGE_SIZE)
        {
            hva = (char *) p->hva + (gpa - p->gpa);
            break;
        }

        p = p->next;
    }

    return hva;
}

uint64_t get_pml4_gpa()
{
    return pml4_gpa;
}

int
vm_mem_init()
{
    uint64_t gpa;

    if (vm_mem_alloc(16, &gpa) == NULL)
    {
        fprintf(stderr, "vm_mem_init(): vm_mem_alloc failed.\n");
        goto ERROR;
    }

    if (gpa != 0)
    {
        fprintf(stderr, "vm_mem_init(): gpa != 0\n");
        goto ERROR;
    }

    if (map_gva(0, 16, gpa))
    {
        fprintf(stderr, "vm_mem_init(): map_gva failed\n");
        goto ERROR;
    }

    vm_mem_dump();

    return 0;
ERROR:
    return -1;
}

void
vm_mem_destroy()
{
    // TODO
}

int
vm_mem_read(uint64_t gpa, void* buf, size_t bytes)
{
    uint8_t *out = (uint8_t *) buf;
    int bytes_read = 0;

    while (bytes_read < bytes)
    {
        uint8_t *hva = (uint8_t *) gpa_to_hva(gpa);

        if (hva == NULL)
        {
            fprintf(stderr, "vm_mem_read(): addr (0x%"PRIx64") out of bounds\n",
                    gpa);
            break;
        }

        *out = *hva;
        bytes_read++;
        out++;
        gpa++;
    }

    return bytes_read;
}

int
vm_mem_write(uint64_t gpa, const void* buf, size_t bytes)
{
    uint8_t *in = (uint8_t *) buf;
    int bytes_written = 0;

    while (bytes_written < bytes)
    {
        uint8_t *hva = (uint8_t *) gpa_to_hva(gpa);

        if (hva == NULL)
        {
            fprintf(stderr, "vm_mem_write(): addr (0x%"PRIx64") out of bounds\n",
                    gpa);
            break;
        }

        *hva = *in;
        bytes_written++;
        in++;
        gpa++;
    }

    return bytes_written;
}
