#ifndef HD_H
#define HD_H

#include <kernel/drivers/pci.h>
#include <kernel/drive.h>

enum {
    IDE_CONTROLLER    = 0x01,
    FLOPPY_CONTROLLER = 0x02,
    SATA_CONTROLLER   = 0x06
};

void hd_init(pci_dev_t **pci_devices, int pci_dev_amt);
controller_t *hd_find_controller(int pci_type);

#endif