#ifndef MM_H
#define MM_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
	uint8_t status;
	uint32_t size;
} alloc_t;

void mm_init(uint32_t kernel_end);
void mm_print_out();

void* pmalloc(size_t size); /* page aligned alloc */
void* malloc(size_t size);
void free(void *mem);

#endif