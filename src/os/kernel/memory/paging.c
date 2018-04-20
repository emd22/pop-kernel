#include <kernel/memory/paging.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <osutil.h>

#ifdef OS_x86
struct {
    uint32_t *pdir;
    uint32_t pdir_loc;
    uint32_t *ptab;
} page_inf;

void paging_map(uint32_t virt, uint32_t phys) {
    if (virt < 0 || phys < 0) {
        printf("error: trying to map paging using unsigned number.\n");
        return;
    }

    uint16_t id = virt >> 22;
    int i;
    for (i = 0; i < 1024; i++) {
        page_inf.ptab[i] = phys | 3;
        phys += 4096;
    }
    page_inf.pdir[id] = ((uint32_t)page_inf.ptab) | 3;
    page_inf.ptab = (uint32_t *)(((uint32_t)page_inf.ptab)+4096);
    printf("Mapping 0x%x (%d) to 0x%x\n", virt, id, phys);
}

void paging_init() {
    memset(&page_inf, 0, sizeof(page_inf));

    page_inf.pdir = (uint32_t *)0x400000;
    page_inf.pdir_loc = (uint32_t)page_inf.pdir;
    page_inf.ptab = (uint32_t *)0x404000;

    memset(page_inf.pdir, 0 | 2, 1024);

    paging_map(0, 0);
    paging_map(0x400000, 0x400000);

    asm volatile("mov %%eax, %%cr3": :"a"(page_inf.pdir_loc));	
    asm volatile("mov %cr0, %eax");
    asm volatile("or %eax, 0x80000001");
    asm volatile("mov %eax, %cr0");
}
#elif defined(OS_x64)
struct {
    uint64_t pdir_ptr_table[4] __attribute__((aligned(0x20)));
    uint64_t pdir[512] __attribute__((aligned(0x1000)));
    uint64_t ptab[512] __attribute__((aligned(0x1000)));
} page_inf;

void paging_init() {
    page_inf.pdir_ptr_table[0] = (uint64_t)&page_inf.pdir | 1;
    page_inf.pdir[0] = (uint64_t)&page_inf.ptab | 3;

    unsigned i, addr = 0;
    for (i = 0; i < 512; i++) {
        page_inf.ptab[i] = addr | 3; // map address and mark it present/writable
        addr = addr+0x1000;
    }

    paging_map(0, 0);
    paging_map(0x400000, 0x400000);

    asm volatile ("mov %cr4, %rax; bts $5, %rax; mov %rax, %cr4"); // set bit5 in CR4 to enable PAE		 
    asm volatile ("mov %%rax, %%cr3" :: "a" (&page_inf.pdir_ptr_table)); // load PDPT into CR3
    asm volatile ("mov %cr0, %rax; orl $0x80000000, %eax; mov %rax, %cr0;");
}

void paging_map(uint32_t virt, uint32_t phys) {
    if (virt < 0 || phys < 0) {
        printf("error: trying to map paging using unsigned number.\n");
        return;
    }

    page_inf.pdir_ptr_table[0] = (uint64_t)&page_inf.pdir | 1;
    page_inf.pdir[0] = (uint64_t)&page_inf.ptab | 3;

    uint16_t id = virt >> 22;
    int i;

    for (i = 0; i < 512; i++) {
        page_inf.ptab[i] = phys | 3; // map address and mark it present/writable
        phys += 4096;
    }
    printf("Mapping 0x%x (%d) to 0x%x\n", virt, id, phys);
    
    // page_inf.pdir[id] = ((uint32_t)page_inf.ptab) | 3;
    // page_inf.ptab = (uint32_t *)(((uint32_t)page_inf.ptab)+4096);
}

// void paging_map(uint32_t virt, uint32_t phys) {
//     if (virt < 0 || phys < 0) {
//         printf("error: trying to map paging using unsigned number.\n");
//         return;
//     }

//     uint16_t id = virt >> 22;
//     int i;

//     for (i = 0; i < 1024; i++) {
//         page_inf.page_prev[i] = phys | 3;
//         phys += 4096;
//     }
//     page_inf.pdir[id] = ((uint32_t)page_inf.page_prev) | 3;
//     page_inf.page_prev = (uint32_t *)(((uint32_t)page_inf.page_prev)+4096);
//     printf("Mapping 0x%x (%d) to 0x%x\n", virt, id, phys);
// }

#endif