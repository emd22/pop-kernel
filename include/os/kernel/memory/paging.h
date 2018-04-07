#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

void paging_map(uint32_t virt, uint32_t phys);
void paging_init();

#endif