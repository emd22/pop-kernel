#ifndef PCI_H
#define PCI_H

#include <stdint.h>
#include <stdbool.h>

#define PCI_SUCCESS 1
#define PCI_FAIL 0

#define PCI_MULTI_FUNCTION(id) (id & (1 << 7))

#define GET_DEVICE_ID(bus, slot) pci_readw(bus, slot, 0, 2);
#define GET_VENDOR_ID(bus, slot) pci_readw(bus, slot, 0, 0);

uint64_t pci_brute_force(void);

#endif