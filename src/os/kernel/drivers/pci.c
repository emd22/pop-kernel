#include <kernel/drivers/pci.h>

void pci_read(uint8_t bus, uint8_t device, uint8_t function) {
    outl((1 << 31) | (bus << 16) | (device << 11) | (func << 8) | 8, 0xCF8);
    if ((inl(0xCFC) >> 16) == 0xFFFF) {
        continue;
    }
    
}