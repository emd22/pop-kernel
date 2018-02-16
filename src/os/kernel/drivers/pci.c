#include <kernel/drivers/pci.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define PCI_AMT 12

#define BUS_AMT 256
#define SLOT_AMT 32

#define NEXT -1

struct {
    pci_header_t *hdrs;
    unsigned index;
} hdrs;

void pci_init(void) {
    hdrs.hdrs = (pci_header_t *)malloc(sizeof(pci_header_t)*8192);
    hdrs.index = 0;
    
    //set all values of headers to zero
    int i;
    for (i = 0; i < PCI_AMT; i++)
        memset(&hdrs.hdrs[i], 0, sizeof(pci_header_t));

    scan_brute_force();

    printf("FOUND %d PCI DEVICES!\n", hdrs.index);
}

inline void sysoutl(uint16_t port, uint32_t val) {
    asm volatile("outl %0, %1"
            : : "a"(val), "Nd"(port));
}

inline uint32_t sysinl(uint16_t port) {
    uint32_t ret = 0;
    asm volatile("outl %0, %1"
            : : "a"(ret), "Nd"(port));
    return ret;
}

uint16_t pci_config_readw(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t addr;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;

    addr = (uint32_t)((lbus << 16) | (lslot << 11) |
           (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    //write out the address
    sysoutl(0xCF8, addr);

    //read data and return it as uint16
    return (uint16_t)((sysinl(0xCFC) >> ((offset & 2)*8)));
}

uint16_t pci_check_vendor(uint8_t bus, uint8_t slot) {
    uint16_t vendor, device;
    //try and read the first configuration register. 0xFFFF = non-existent device.
    if ((vendor = (pci_config_readw(bus, slot, 0, 0) & 0xFFFF)) != 0xFFFF) {
        device = pci_config_readw(bus, slot, 0, 2) & 0xFFFF;
    }
    //vendor code exists, so we'll return it
    return vendor;
}

bool dev_is_valid(uint8_t bus, uint8_t slot, uint8_t func) {
    uint16_t vendor = pci_config_readw(bus, slot, func, 0) & 0xFFFF;

    if (vendor == 0xFFFF)
        return false;

    return true;
}

void create_dev_header(uint8_t bus, uint8_t slot, uint8_t func) {
    pci_header_t *head = &hdrs.hdrs[hdrs.index];
    head->bus      = bus;
    head->slot     = slot;
    head->function = func;

    uint8_t head_type = (pci_config_readw(bus, slot, func, 0x0C) & 0xFF0000) >> 16;
    head->head_type = head_type & 0x7F;
    head->multi_func = (head_type & 0x80) >> 7;

    uint32_t entry00 = pci_config_readw(bus, slot, func, 0);
    head->dev_id = entry00 >> 16;
    head->vend_id = entry00 & 0xFFFF;

    uint32_t entry08 = pci_config_readw(bus, slot, func, 0x08);
    head->class_ = (entry08 & 0xFF000000) >> 24;
    head->subclass = (entry08 & 0xFF0000) >> 16;
    head->prog_if = (entry08 & 0xFF00) >> 8;
    head->rev_id = (entry08 & 0xFF);
    hdrs.index++;
}

void set_hdr(int index, uint8_t bus, uint8_t slot, uint8_t func) {
    pci_header_t *hdr = &hdrs.hdrs[index == -1 ? index : hdrs.index];
}

bool chk_multi_func(uint8_t bus, uint8_t slot) {
    uint8_t head_type = (pci_config_readw(bus, slot, 0, 0x0C) & 0xFF0000) >> 16;
    return (head_type & 0x80);
}

void check_function(uint8_t bus, uint8_t dev, uint8_t func) {
    uint8_t class_, subclass, bus2 = 0;

    pci_header_t *head = find_dev(bus, dev, func);

    class_ = head->class_;
    subclass = head->subclass;
}

void check_dev(uint8_t bus, uint8_t dev) {
    uint8_t func = 0;

    pci_header_t *head = find_dev(bus, dev, func);

    uint8_t vend_id = head->vend_id;

    if (vend_id == 0xFFFF)
        return;

    check_function(bus, dev, func);
    
    if (!(head->head_type & 0x80)) {
        //multi function device, check remaining functions
        for (func = 1; func < 8; func++) {
            if (head->vend_id != 0xFFFF) {
                hdrs.index++;
                check_function(bus, dev, func);
            }
        }
    }
}

void scan_brute_force(void) {
    int bus, dev;

    for (bus = 0; bus < 256; bus++) {
        for (dev = 0; dev < 32; dev++) {
            check_dev((uint8_t)bus, (uint8_t)dev);
        }
    }
}

// void scan_brute_force(void) {
//     int bus, slot, func;

//     for (bus = 0; bus < BUS_AMT; bus++) {
//         for (slot = 0; slot < SLOT_AMT; slot++) {
//             if (dev_is_valid(bus, slot, 0)) {
//                 if (chk_multi_func(bus, slot)) {
//                     for (func = 0; func < 8; func++) {
//                         if (dev_is_valid(bus, slot, func))
//                             create_dev_header(bus, slot, func);
//                     }
//                 }
//                 else
//                     create_dev_header(bus, slot, 0);
//             }
//         }
//     }
// }

pci_header_t *find_dev(uint8_t bus, uint8_t slot, uint8_t func) {
    pci_header_t hdr;
    
    int i;
    for (i = 0; i < hdrs.index; i++) {
        hdr = hdrs.hdrs[i];

        if (hdr.bus == bus &&
            hdr.slot == slot &&
            hdr.function == func)
        {
            return &hdrs.hdrs[i];
        }
    }
    return NULL;
}