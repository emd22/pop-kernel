#include <kernel/drivers/pci.h>

#include <kernel/arch_io.h>
#include <string.h>

#include <stdio.h>

#define PCI_EMPTY 0xFFFF

#define SECONDARY_BUS(bus, slot, func) (uint8_t)(pci_inw(bus, slot, func, 0x18) >> 8)
#define CLASS_CODE(bus, slot, func)    (uint8_t)(pci_inw(bus, slot, func, 0x0A) >> 8)
#define SUBCLASS_CODE(bus, slot, func) (uint8_t)(pci_inw(bus, slot, func, 0x0A))
#define DEVICE_ID(bus, slot, func)              (pci_inw(bus, slot, func, 0x02))
#define VENDOR_ID(bus, slot, func)              (pci_inw(bus, slot, func, 0x00))
#define HEADER_TYPE(bus, slot, func)   (uint8_t)(pci_inw(bus, slot, func, 0x0E))
#define PROG_IF(bus, slot, func)       (uint8_t)(pci_inw(bus, slot, func, 0x08) >> 8)
#define BASE_ADDR(bus, slot, func, addr_n)      (pci_inl(bus, slot, func, 0x10+(addr_n*4)))

#define PCI_MASS_STORAGE 0x01

#define PCI_IDE_CONTROLLER 0x01

static pci_dev_t **pci_devices = NULL;
static int pci_devices_idx = 0;

void pci_check_device(uint8_t bus, uint8_t slot);

void pci_init(void) {
    pci_devices = (pci_dev_t **)malloc(sizeof(pci_dev_t)*32);
    int i;
    for (i = 0; i < 32; i++) {
        pci_devices[i] = (pci_dev_t *)malloc(sizeof(pci_dev_t));
    }
}

uint16_t pci_inw(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t config_addr;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    
    config_addr = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
    outl(0xCF8, config_addr);
    
    uint16_t out = (uint16_t)((inl(0xCFC) >> ((offset & 2)*8)) & PCI_EMPTY);
    return out;
}

uint32_t pci_inl(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint16_t p1 = pci_inw(bus, slot, func, offset);
    uint16_t p2 = pci_inw(bus, slot, func, offset+0x02);
    return (p1 << 16) | p2;
}

void pci_check_bus(uint8_t bus) {
    uint8_t slot;
    for (slot = 0; slot < 32; slot++) {
        pci_check_device(bus, slot);
    }
}

void pci_check_function(uint8_t bus, uint8_t slot, uint8_t func) {
    uint8_t class_id = CLASS_CODE(bus, slot, func);
    uint8_t subclass = SUBCLASS_CODE(bus, slot, func);
    uint8_t secondary_bus;

    pci_dev_t *dev = pci_devices[pci_devices_idx++];
    dev->vendor_id = VENDOR_ID(bus, slot, func);
    dev->device_id = DEVICE_ID(bus, slot, func);
    dev->class_code = class_id;
    dev->subclass_code = subclass;
    dev->prog_if = PROG_IF(bus, slot, func);
    int i;
    for (i = 0; i < 6; i++) {
        dev->bars[i] = BASE_ADDR(bus, slot, func, i);
    }

    // printf("class: %d, subclass: %d\n", dev->class_code, dev->subclass_code);

    if ((class_id == 0x06) && (subclass == 0x04)) {
        secondary_bus = SECONDARY_BUS(bus, slot, func);
        pci_check_bus(secondary_bus);
    }
}

void pci_check_device(uint8_t bus, uint8_t slot) {
    uint8_t function = 0;
    uint16_t vendor_id = VENDOR_ID(bus, slot, 0);

    if (vendor_id == PCI_EMPTY)
        return;

    pci_check_function(bus, slot, 0);

    if ((HEADER_TYPE(bus, slot, function) & 0x80) != 0) {
        for (function = 1; function < 8; function++) {
            if (VENDOR_ID(bus, slot, function) != 0xFFFF) {
                pci_check_function(bus, slot, function);
            }
        }
    }
}

void pci_recursive_check(void) {
    uint8_t function;

    pci_devices_idx = 0;

    uint8_t header_type = HEADER_TYPE(0, 0, 0);
    if ((header_type & 0x80) == 0) {
        //single pci host controller
        pci_check_bus(0);
    }
    else {
        //multiple pci host controllers
        for (function = 0; function < 8; function++) {
            if (VENDOR_ID(0, 0, function) != 0xFFFF)
                break;
            pci_check_bus(function);
        }
    }
}

pci_dev_t **pci_get_devices(int *amt_devices) {
    (*amt_devices) = pci_devices_idx;
    return pci_devices;
}