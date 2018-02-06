#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>


struct _mem_page {
    struct _mem_page *page_next;

    uint16_t page_ptrc;
};

typedef _mem_page mem_page_t;


#endif