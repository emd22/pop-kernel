#include <osutil.h>

#include <kernel/hd.h>
#include <kernel/drive.h>
#include <kernel/drivers/ide.h>
#include <kernel/drivers/pci.h>

#include <string.h>

enum {
    MASS_STORAGE_DEVICE = 0x01
};

static pci_dev_t *pci_devs;
static int selected_hd_controller = 0;
static int selected_hd = 0;
static controller_t controllers[32];
static int controllers_index = 0;

void ide_controller_init(int pci_index) {
    controller_t controller;
    memset(&controller, 0, sizeof(controller_t));
    controller.pci_type    = IDE_CONTROLLER;
    controller.pci_index   = pci_index;
    controller.read_block  = ide_read_block;
    controller.write_block = ide_write_block;

    /* controller.drives = (void *) */ide_init(&controller);

    controllers[controllers_index++] = controller;
}

void check_class_code(pci_dev_t *cur_dev, int dev_index) {
    switch (cur_dev->class_code) {
        case MASS_STORAGE_DEVICE:
            switch (cur_dev->subclass_code) {
                case IDE_CONTROLLER:
                    ide_controller_init(dev_index);

                    break;
            }
            break;
    }
}

void hd_init(pci_dev_t **pci_devices, int pci_dev_amt) {
    int i;
    pci_dev_t *cur_dev;
    for (i = 0; i < pci_dev_amt; i++) {
        cur_dev = pci_devices[i];
        check_class_code(cur_dev, i);
    }
}

controller_t *hd_find_controller(int pci_type) {
    int i;
    controller_t *controller;
    for (i = 0; i < controllers_index; i++) {
        controller = &controllers[i];
        if (controller->pci_type == pci_type) {
            printf("%d: %d == %d\n", i, controller->pci_type, pci_type);
            return controller;
        }
    }
    return NULL;
}

void hd_select(int hd) {
    selected_hd = hd;
}