#ifndef SYSASM_H
#define SYSASM_H

#include <stdint.h>

uint8_t inb(uint16_t port);
void    outb(uint16_t port, uint8_t val);
uint8_t inw(uint16_t port);
void    outw(uint16_t port, uint8_t val);

#endif