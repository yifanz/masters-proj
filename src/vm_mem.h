/*
 * Copyright (c) 2016, Yi-Fan Zhang.  All rights reserved.
 * Copyrights licensed under the BSD License.
 * See the accompanying LICENSE file for terms.
 * See the accompanying ATTRIB file for attributions/credits.
 */
#ifndef vm_mem_h
#define vm_mem_h

#include <stdint.h>
#include <sys/mman.h>
#include <Hypervisor/hv.h>
#include <Hypervisor/hv_vmx.h>

void vm_mem_dump();

void* vm_mem_alloc(size_t npages, uint64_t *gpa);

int map_gpa(uint64_t gpa, size_t npages, void* hva);
int unmap_gpa(uint64_t gpa, size_t npages);
int map_gva(uint64_t gva, size_t npages, uint64_t gpa);
int unmap_gva(uint64_t gva, size_t npages);

uint64_t gva_to_gpa(uint64_t gva);
void* gpa_to_hva(uint64_t gpa);

uint64_t get_pml4_gpa();

int vm_mem_init();
void vm_mem_destroy();

int vm_mem_read(uint64_t gpa, void* buf, size_t bytes);
int vm_mem_write(uint64_t gpa, const void* buf, size_t bytes);

int vm_mem_verify(uint64_t gva, size_t nbytes);
#endif /* vm_mem_h */
