#ifndef ARCH_IO_H
#define ARCH_IO_H

#include <stdint.h>

#define SYSCALL_VECTOR 0x7F

inline uint8_t  inb(uint16_t port);
inline void     outb(uint16_t port, uint8_t val);
inline uint16_t inw(uint16_t port);
inline void     outw(uint16_t port, uint16_t val);
inline unsigned inl(uint16_t port);
inline void     outl(uint16_t port, unsigned data);

#endif