#include <kernel/drivers/pci.h>

#include <kernel/arch_io.h>
#include <string.h>

#define PCI_EMPTY 0xFFFF

#define GET_HEADER_TYPE(bus, device, func) (pci_inw(bus, device, ))

typedef struct {
    uint32_t unused       : 2;
    uint32_t field_num    : 6;
    uint32_t function_num : 3;
    uint32_t device_num   : 5;
    uint32_t bus_num      : 8;
    uint32_t reserved     : 7;
    uint32_t enable       : 1;
} pci_dev_t;

// typedef struct {
//     uint16_t vendor_id;
//     uint16_t device_id;
//     uint8_t class_id;
//     uint8_t subclass;
//     uint8_t secondary_bus;
//     uint8_t primary_bus;
//     uint8_t BIST;
//     uint8_t header_type;
// } pci_device_t;

void pci_check_device(uint8_t bus, uint8_t device);

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

uint16_t pci_device_type(uint8_t bus, uint8_t device, uint8_t func) {
    uint16_t type;
    type = (pci_inw(bus, device, func, ))
}

pci_device_t *pci_get_info(pci_device_t *pci_dev, uint8_t bus, uint8_t slot) {
    pci_dev->vendor = pci_inw(bus, slot, 0, 0);
    
    if (pci_dev->vendor != PCI_EMPTY) { //if device exists(has vendor)
        pci_dev->device = pci_inw(bus, slot, 0, 2);

        //get 16 bits(8 bits subclass, 8 bits class), split to two values
        uint16_t class_subclass = pci_inw(bus, slot, 2, 8);
        pci_dev->class_id = (uint8_t)class_subclass;
        pci_dev->subclass = (uint8_t)(class_subclass << 8);
        
        uint16_t bist_header = pci_inw(bus, slot, 3, 12);
        pci_dev->header_type = (uint8_t)bist_header;
        pci_dev->BIST = (uint8_t)(bist_header << 8);

        uint16_t bus_numbers = pci_inw(bus, slot, 6, 26);
        pci_dev->primary_bus = (uint8_t)bus_numbers;
        pci_dev->secondary_bus = (uint8_t)(bus_numbers << 8);
    
    }
    else {
        //garbage values are for nerds.
        memset(pci_dev, 0, sizeof(pci_device_t));
    }

    return pci_dev;
}

void pci_check_bus(uint8_t bus) {
    uint8_t device;
    for (device = 0; device < 32; device++) {
        pci_check_device(bus, device);
    }
}

void pci_check_function(pci_device_t *pci_dev, uint8_t bus, uint8_t slot, uint8_t func) {
    uint8_t class_id = pci_dev->class_id;
    uint8_t subclass = pci_dev->subclass;
    uint8_t secondary_bus;

    if (class_id == 0x06 && subclass == 0x04) {
        secondary_bus = pci_dev->secondary_bus;
        pci_check_bus(secondary_bus);
    }
}

void pci_check_device(uint8_t bus, uint8_t device) {
    uint8_t function = 0;

    pci_device_t pci_dev;
    pci_get_info(&pci_dev, bus, device);

    if (pci_dev->vendor_id == PCI_EMPTY)
        return;
    
    pci_check_function(pci_dev, bus, device, 0);
    if ((pci_dev->header_type & 0x80) != 0) {
        for (function = 1; function < 8; function++) {
            pci_get_info(&pci_dev, bus, device);
            if (pci_dev->vendor_id != 0xFFFF) {
                pci_check_function(pci_dev, bus, device, function);
            }
        }
    }
}



void pci_check_busses(void) {
    uint8_t bus, function;

}