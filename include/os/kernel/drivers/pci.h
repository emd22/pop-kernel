#ifndef PCI_H
#define PCI_H

#include <stdint.h>
#include <stdbool.h>

#define PCI_REQ_CLASS_SUB 0x00
#define PCI_REQ_VEND_DEV  0x01

typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t class_code;
    uint8_t subclass_code;
} pci_dev_t;

void pci_init(void);
int pci_get_devices(pci_dev_t *pci_devices_ptr);
void pci_recursive_check(void);
void pci_brute_force(void);

#endif