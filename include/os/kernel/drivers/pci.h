#ifndef PCI_H
#define PCI_H

#include <stdint.h>

typedef struct {
    uint16_t vendorid;
    uint8_t bus;
    uint8_t dev;
    uint8_t func;
    uint8_t class_;
    uint8_t subclass;
} pci_dat_t;

typedef struct {
    uint16_t vendorid;
    uint8_t bus;
    uint8_t dev;
    uint8_t func;
    uint8_t class_;
    uint8_t subclass;
} pci_function_t;

#define PCI_C_STORAGE 0x10
#define PCI_SC_AHCI 0x60

unsigned pcireadl(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);
uint16_t pcireadw(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);
uint8_t pcireadb(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);

#endif