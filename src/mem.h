#ifndef mem_h
#define mem_h

#include <stdint.h>

int vm_mem_init();
void vm_mem_destroy();

int rmem(uint64_t offset, void* buf, int bytes);
uint8_t rmem8(uint64_t offset);
uint16_t rmem16(uint64_t offset);
uint32_t rmem32(uint64_t offset);
uint64_t rmem64(uint64_t offset);

int wmem(uint64_t offset, void* buf, int bytes);
int wmem8(uint64_t offset, uint8_t in);
int wmem16(uint64_t offset, uint16_t in);
int wmem32(uint64_t offset, uint32_t in);
int wmem64(uint64_t offset, uint64_t in);

uint64_t gva_to_gpa(uint64_t gva);
void* vm_mem_ptr(uint64_t offset);

#endif /* mem_h */
