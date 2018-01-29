#ifndef SYSASM_H
#define SYSASM_H

#include <stdint.h>
#include <stddef.h>

inline uint8_t  inb(uint16_t port);
inline void     outb(uint16_t port, uint8_t val);
inline uint16_t inw(uint16_t port);
inline void     outw(uint16_t port, uint16_t val);

//outsw -> send a string of shorts(2 bytes)
inline void     outsb(uint16_t port, uint8_t *buffer, size_t count);
inline void     insb(uint16_t port, uint8_t *buffer, size_t count);
// inline void     insl(uint16_t port, uint8_t *buffer, size_t count);
// inline void     outsl(uint16_t port, uint8_t *buffer, size_t count);
// inline void     outsm(unsigned short port, unsigned char *data, unsigned long size);
// inline void     insm(unsigned short port, unsigned char *data, unsigned long size);

#endif