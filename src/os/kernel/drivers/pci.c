#include <kernel/drivers/pci.h>

uint16_t pci_readw(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint64_t tmp = 0;

    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    outl(0xCF8, address);

    tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2)*8)) & 0xFFFF);
    return tmp;
}

void pci_check_bus(uint8_t bus) {
    uint8_t device, function;
    for (device = 0; device < 32; device++) {
        int vendor_id = GET_VENDOR_ID(bus, device);
        if (PCI_MULTI_FUNCTION(vendor_id)) {
            for (function = 0; function < 8; function++) {
                pci_check_function(bus, device, function);
            }
        } else {
            pci_check_function(bus, device, 0);
        }
    }
}

void pci_host_controller(uint8_t bus, uint8_t device, uint8_t function) {
    pci_check_bus(GET_SECONDARY_BUS(bus, device, function));
}

void pci_to_bridge(uint8_t bus, uint8_t device, uint8_t function) {
    pci_check_bus(GET_SECONDARY_BUS(bus, device, function));    
}

void pci_check_function(uint8_t bus, uint8_t device, uint8_t function) {
    int type;

    if (bus == 0 && device == 0)
        pci_host_controller(bus, device, function);
    else {
        type = GET_CLASS_CODE(bus, device, function);
        switch (type) {
            case PCI_BRIDGE_DEVICE:
                pci_to_bridge(bus, device, function);
                break;
        };
    }
}

bool pci_check_device(uint8_t bus, uint8_t device) {
    uint8_t function = 0;
    
    uint16_t vendor_id, device_id;
    vendor_id = GET_VENDOR_ID(bus, device);
    if (vendor_id == 0xFFFF)
        return DEVICE_NOT_EXIST;

    return DEVICE_EXISTS;
}

uint64_t pci_brute_force(void) {
    pci_check_bus(0);
    return 0;
}

// uint64_t pci_brute_force(void) {
//     uint16_t bus;
//     uint8_t device;
//     uint32_t bar5;

//     uint16_t vendor_id, device_id;

//     for (bus = 0; bus < 256; bus++) {
//         for (device = 0; device < 32; device++) {
//             vendor_id = GET_VENDOR_ID(bus, device);

//             if (vendor_id == 0xFFFF)
//                 return 0; //device does not exist

//             device_id = GET_DEVICE_ID(bus, device);

//             if (PCI_MULTI_FUNCTION(vendor_id)) {
//                 bar5 = pci_readw(bus, device, 0, (0x24));
//                 printf("vendor: 0x%x device: 0x%x, b: %d, d: %d -> %d\n", vendor_id, device_id, bus, device, bar5);
//                 return bar5;
//             }
//         }
//     }
//     return 0;
// }