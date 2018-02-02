#ifndef x86_H
#define x86_H

//x86 asm functions(inb, outb, etc.)

#include <stdint.h>
#include <stddef.h>

inline uint8_t  inb(uint16_t port);
inline void     outb(uint16_t port, uint8_t val);
inline uint16_t inw(uint16_t port);
inline void     outw(uint16_t port, uint16_t val);
inline uint32_t readl(const void *addr);
inline void     outl(const void *addr, uint32_t val);

#endif