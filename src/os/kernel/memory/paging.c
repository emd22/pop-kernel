#include <kernel/memory/memory.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

struct {
    uint32_t *pdir;
    uint32_t pdir_loc;
    uint32_t *page_prev;
} page_inf;

void paging_map(uint32_t virt, uint32_t phys) {
    uint16_t id = virt >> 22;
    int i;
    for (i = 0; i < 1024; i++) {
        page_inf.page_prev[i] = phys | 3;
        phys += 4096;
    }
    page_inf.pdir[id] = ((uint32_t)page_inf.page_prev) | 3;
    page_inf.page_prev = (uint32_t *)(((uint32_t)page_inf.page_prev)+4096);
    printf("Mapping 0x%x (%d) to 0x%x\n", virt, id, phys);
}

void paging_init() {
    memset(&page_inf, 0, sizeof(page_inf));
    printf("HOI\n");

    page_inf.pdir = (uint32_t *)0x400000;
    page_inf.pdir_loc = (uint32_t)page_inf.pdir;
    page_inf.page_prev = (uint32_t *)0x404000;

    memset(page_inf.pdir, 0 | 2, 1024);

    paging_map(0, 0);
    paging_map(0x400000, 0x400000);

    asm volatile("mov %%eax, %%cr3": :"a"(page_inf.pdir_loc));	
    asm volatile("mov %cr0, %eax");
    asm volatile("or %eax, 0x80000001");
    asm volatile("mov %eax, %cr0");
}