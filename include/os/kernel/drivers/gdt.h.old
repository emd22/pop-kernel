#ifndef GDT_H
#define GDT_H

#include <stddef.h>

void gdt_set_gate(int num, size_t base, size_t limit, char access, char gran);
void gdt_install(void);

#endif