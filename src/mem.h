#ifndef mem_h
#define mem_h

#include <stdint.h>

uint16_t rmem16(uintptr_t offset, int* err);
uint64_t gva_to_gpa(uint64_t gva);

#endif /* mem_h */
