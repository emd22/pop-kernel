#include <kernel/sysasm.h>

inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile("inw %1, %0"
                   : "=a"(ret)
                   : "Nd"(port));
    return ret;
}

inline void outw(uint16_t port, uint16_t val) {
    asm volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}