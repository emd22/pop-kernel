#ifndef MM_H
#define MM_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
	uint8_t status;
	uint32_t size;
} alloc_t;

typedef struct {
    uint32_t last_alloc;
    uint32_t heap_end;
    uint32_t heap_begin;
    uint32_t pheap_begin;
    uint32_t pheap_end;
    uint8_t *pheap_desc;
    uint32_t memory_used;
} mm_inf_t;

void mm_init(uint32_t kernel_end);
void mm_print_out();

void* pmalloc(size_t size); /* page aligned alloc */
void* malloc(size_t size);
void free(void *mem);

mm_inf_t get_mm_inf(void);

#endif