#ifndef PCI_H
#define PCI_H

#include <stdint.h>
#include <stdbool.h>

#define DEVICE_EXISTS true
#define DEVICE_NOT_EXIST false

#define PCI_MULTI_FUNCTION(id) (id & (1 << 7))

#define GET_DEVICE_ID(bus, slot) pci_readw(bus, slot, 0, 2);
#define GET_VENDOR_ID(bus, slot) pci_readw(bus, slot, 0, 0);
#define GET_SECONDARY_BUS(bus, slot, function) (uint8_t)(pci_readw(bus, slot, function, 0x18) >> 8)
#define GET_CLASS_CODE(bus, slot, function) (uint8_t)pci_readw(bus, slot, 0, 0x0B);
#define GET_SUB_CLASS_CODE(bus, slot, function) (uint8_t)pci_readw(bus, slot, 0, 0x0A);

#define PCI_BRIDGE_DEVICE 0x06
#define PCI_NO_CLASS 0xFF
#define PCI_MASS_STORAGE_CONTROLLER 0x01
#define PCI_NO_DEVICE 0x00

typedef struct {
    uint8_t bus;
    uint8_t slot;
    int type;
} pci_device_t;

uint64_t pci_brute_force(void);
uint16_t pci_readw(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

#endif