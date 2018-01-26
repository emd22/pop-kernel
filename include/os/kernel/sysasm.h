#ifndef SYSASM_H
#define SYSASM_H

#include <stdint.h>

inline uint8_t inb(uint16_t port);
inline void    outb(uint16_t port, uint8_t val);
inline uint16_t inw(uint16_t port);
inline void    outw(uint16_t port, uint16_t val);

#endif