// #include <kernel/drivers/pci.h>
// #include <kernel/drivers/ide.h>

// enum {
//     MASS_STORAGE_DEVICE = 0x01
// };

// enum {
//     IDE_CONTROLLER    = 0x01,
//     FLOPPY_CONTROLLER = 0x02,
//     SATA_CONTROLLER   = 0x06
// };

// typedef struct {
//     int type;
//     int pci_index;
// } hd_dev_t;

// static hd_dev_t hd_devs[16];
// static pci_dev_t *pci_devs;
// static int selected_hd = 0;

// void check_class_code(pci_dev_t *cur_dev, int dev_index) {
//     hd_dev_t hd;

//     switch (cur_dev.class_code) {
//         case MASS_STORAGE_DEVICE:
//             switch (cur_dev.subclass_code) {
//                 case IDE_CONTROLLER:
//                     if (ide_init() == -1) {
//                         break;
//                     }
//                     hd.type = IDE_CONTROLLER;
//                     hd.pci_index = i;
//                     break;
//             }
//             break;
//     }
// }

// void hd_init(pci_dev_t *pci_devices, int pci_dev_amt) {
//     pci_devs = pci_devices;

//     int i;
//     pci_dev_t cur_dev;
//     for (i = 0; i < pci_dev_amt; i++) {
//         cur_dev = pci_devices[i];
//         check_class_code(&cur_dev, i); 
//     }
// }

// void hd_select(int hd) {
//     selected_hd = hd;
// }