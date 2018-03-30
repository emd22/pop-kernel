#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

void paging_map_virt_phys(uint32_t virt, uint32_t phys);
void paging_init();

#endif