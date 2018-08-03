#include <kernel/drivers/pci.h>

#include <kernel/arch_io.h>

#define PCI_EMPTY 0xFFFF

int pci_read(uint8_t bus, uint8_t device, uint8_t function) {
    outl(0xCF8, (1 << 31) | (bus << 16) | (device << 11) | (func << 8) | 8);
    if ((inl(0xCFC) >> 16) == PCI_EMPTY) {
        //if no device found, return.
        return;
    }
    //check if this device needs an IRQ assignment
    outl(0xCF8, (1 << 31) | (bus << 16) | (device << 11) | (func << 8) | 0x3C); // Read the interrupt line field
    outb(0xCFC, 0xFE); // Change the IRQ field to 0xFE
    outl(0xCF8, (1 << 31) | (bus << 16) | (device << 11) | (func << 8) | 0x3C); // Read the interrupt line field
    if ((inl(0xCFC) & 0xFF) == 0xFE) {
        //device needs IRQ assignment
    }
}