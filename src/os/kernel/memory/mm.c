#include <kernel/memory/mm.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <osutil.h>

#define MAX_PAGE_ALIGNED_ALLOCS 32

struct {
    uint32_t last_alloc;
    uint32_t heap_end;
    uint32_t heap_begin;
    uint32_t pheap_begin;
    uint32_t pheap_end;
    uint8_t *pheap_desc;
    uint32_t memory_used;
} mm_inf;

void mm_init(uint32_t kernel_end) {
    memset(&mm_inf, 0, sizeof(mm_inf));

    mm_inf.last_alloc = kernel_end + 0x1000;
    mm_inf.heap_begin = mm_inf.last_alloc;
    mm_inf.pheap_end = 0x400000;
    mm_inf.pheap_begin = mm_inf.pheap_end - (MAX_PAGE_ALIGNED_ALLOCS * 4096);
    mm_inf.heap_end = mm_inf.pheap_begin;
    memset((char *)mm_inf.heap_begin, 0, mm_inf.heap_end-mm_inf.heap_begin);
    mm_inf.pheap_desc = (uint8_t *)malloc(MAX_PAGE_ALIGNED_ALLOCS);
}

void free(void *mem) {
    alloc_t *alloc = (mem - sizeof(alloc_t));
    mm_inf.memory_used -= alloc->size + sizeof(alloc_t);
    alloc->status = 0;
}

void *malloc(size_t size) {
    if(!size) return 0;

    uint8_t *mem = (uint8_t *)mm_inf.heap_begin;

    while ((uint32_t)mem < mm_inf.last_alloc) {
        alloc_t *a = (alloc_t *)mem;

        if (!a->size)
            break;
        
        if (a->status) {
            mem += a->size;
            mem += sizeof(alloc_t);
            mem += 4;
            continue;
        }

        if (a->size >= size) {
            a->status = 1;
            memset(mem + sizeof(alloc_t), 0, size);
            mm_inf.memory_used += size+sizeof(alloc_t);
            return (void *)(mem+sizeof(alloc_t));
        }
        mem += a->size;
        mem += sizeof(alloc_t);
        mem += 4;
    }

    if (mm_inf.last_alloc+size+sizeof(alloc_t) >= mm_inf.heap_end) {
        printf("Cannot allocate. %d > %d", mm_inf.last_alloc+size+sizeof(alloc_t), mm_inf.heap_end);
        panic("Cannot allocate bytes. System out of memory.", NULL);
    }

    alloc_t *alloc = (alloc_t *)mm_inf.last_alloc;
    alloc->status = 1;
    alloc->size = size;

    mm_inf.last_alloc += size;
    mm_inf.last_alloc += sizeof(alloc_t);
    mm_inf.last_alloc += 4;
    mm_inf.memory_used += size + 4 + sizeof(alloc_t);

    memset((void *)((uint32_t)alloc + sizeof(alloc_t)), 0, size);
    return (void *)((uint32_t)alloc + sizeof(alloc_t));
}