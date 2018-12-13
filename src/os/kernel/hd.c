#include <osutil.h>

#include <kernel/hd.h>
#include <kernel/drive.h>
#include <kernel/drivers/ide.h>
#include <kernel/drivers/pci.h>

#include <string.h>

enum {
    MASS_STORAGE_DEVICE = 0x01
};

const char *drive_controller_types[] = {
    NULL,
    "PATA Controller",
    "Floppy Controller",
    NULL,
    NULL,
    NULL,
    "SATA Controller",
};

static drive_t drives[32];
static int drive_index = -1;

void ide_drive_init(int pci_index) {
    drive_t *drive = &drives[drive_index++];
    drive->read_block = ide_read_block;
    drive->write_block = ide_write_block;

    drive->controller_type = IDE_CONTROLLER;
    drive->controller_pci_index = pci_index;
}

const char *hd_get_controller_type(drive_t *drive) {
    return drive_controller_types[drive->controller_type];
}

void check_class_code(pci_dev_t *cur_dev, int dev_index) {
    switch (cur_dev->class_code) {
        case MASS_STORAGE_DEVICE:
            switch (cur_dev->subclass_code) {
                case IDE_CONTROLLER:
                    ide_drive_init(dev_index);
                    break;
            }
            break;
    }
}

drive_t *hd_get_drives(int *_drive_index) {
    (*_drive_index) = drive_index;
    return drives;
}

void hd_write_block(unsigned lba, uint16_t sector_count, const uint8_t *data) {
    if (drive_index == -1)
        return;
        
    drives[drive_index].write_block(lba, sector_count, data);
}

void hd_read_block(unsigned lba, uint16_t sector_count, uint8_t *data) {
    if (drive_index == -1)
        return;
    printf("dridx: %d\n", drive_index);

    drives[drive_index].read_block(lba, sector_count, data);
}

void hd_init(pci_dev_t **pci_devices, int pci_dev_amt) {
    int i;
    pci_dev_t *cur_dev;
    for (i = 0; i < pci_dev_amt; i++) {
        cur_dev = pci_devices[i];
        check_class_code(cur_dev, i);
    }
}

// controller_t *hd_find_controller(int pci_type) {
//     int i;
//     controller_t *controller;
//     for (i = 0; i < controllers_index; i++) {
//         controller = &controllers[i];
//         if (controller->pci_type == pci_type) {
//             printf("%d: %d == %d\n", i, controller->pci_type, pci_type);
//             return controller;
//         }
//     }
//     return NULL;
// }

// void hd_select(int hd) {
//     selected_hd = hd;
// }