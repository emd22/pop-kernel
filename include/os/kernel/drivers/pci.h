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
    uint8_t prog_if;
    uint32_t bars[6];
} pci_dev_t;

void pci_init(void);
pci_dev_t **pci_get_devices(int *pci_devices_idx);
void pci_recursive_check(void);
void pci_brute_force(void);

#endif