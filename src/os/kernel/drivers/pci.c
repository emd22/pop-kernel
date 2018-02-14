#include <kernel/drivers/pci.h>
#include <sys/objclr.h>
#include <stdlib.h>
#include <string.h>

#define PCI_AMT 12

pci_header_t *pci_heads;

void pci_init(void) {
    pci_heads = (pci_header_t *)malloc(sizeof(pci_header_t)*PCI_AMT);
    
    //set all values to zero
    int i;
    for (i = 0; i < PCI_AMT; i++)
        memset(&pci_heads[i], 0, sizeof(pci_header_t));
}

inline void sysoutl(uint16_t port, uint32_t val) {
    asm volatile("outl %0, %1"
            : : "a"(val), "Nd"(port));
}

inline uint32_t sysinl(uint16_t port) {
    uint32_t ret;
    asm volatile("outl %0, %1"
            : : "a"(ret), "Nd"(port));
    return ret;
}

uint16_t pci_config_readw(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t addr;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;

    addr = (uint32_t)((lbus << 16) | (lslot << 11) |
           (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    //write out the address
    sysoutl(0xCF8, addr);

    //read data and return it as uint16
    return (uint16_t)((sysinl(0xCFC) >> ((offset & 2)*8)) & 0xFFFF);
}

uint16_t pci_check_vendor(uint8_t bus, uint8_t slot) {
    uint16_t vendor, device;
    //try and read the first configuration register. 0xFFFF = non-existent device.
    if ((vendor = pci_config_readw(bus, slot, 0, 0)) != 0xFFFF) {
        device = pci_config_readw(bus, slot, 0, 2);
    }
    //vendor code exists, so we'll return it
    return vendor;
}

bool dev_is_valid(uint8_t bus, uint8_t slot, uint8_t func) {
    uint16_t vendor = pci_config_readw(bus, slot, func, 0);

    if (vendor == 0xFFFF)
        return false;

    return true;
}

void create_dev_header(uint8_t bus, uint8_t slot, uint8_t func) {

}

void check_dev(uint8_t bus, uint8_t dev) {
    uint8_t func = 0;
    
}