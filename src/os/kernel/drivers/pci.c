#include <kernel/drivers/pci.h>

uint32_t pci_readw(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint64_t tmp = 0;

    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    outl(0xCF8, address);

    if (offset == 0x24)
        tmp = inl(0xCFC);
    else
        tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2)*8)) & 0xFFFF);
    return (tmp);
}

uint64_t pci_brute_force(void) {
    uint16_t bus;
    uint8_t device;
    uint64_t bar5;

    uint16_t vendor_id, device_id;

    for (bus = 0; bus < 256; bus++) {
        for (device = 0; device < 32; device++) {
            vendor_id = GET_VENDOR_ID(bus, device);

            if (vendor_id == 0xFFFF)
                return 0; //device does not exist

            device_id = GET_DEVICE_ID(bus, device);

            if (PCI_MULTI_FUNCTION(vendor_id)) {
                printf("vendor: 0x%x device: 0x%x\n", vendor_id, device_id);
                return pci_readw(bus, device, 0, 0x24);
            }
        }
    }
    return 0;
}