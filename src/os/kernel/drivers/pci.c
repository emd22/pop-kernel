#include <kernel/drivers/pci.h>
#include <kernel/x86.h>

unsigned pcireadl(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset) {
    unsigned long addr;

    addr = (1 << 31) | (bus << 16) | (dev << 11) | (func << 8) | (offset & 0xFC);

    outl(addr, 0xCF8);
    return inl(0xCFC);
}

uint16_t pcireadw(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset) {
    unsigned data;

    data = pcireadl(bus, dev, func, offset);

    return offset % 4 == 0 ? data : data >> 16;
}

uint8_t pcireadb(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset) {
    unsigned data;
    int shift = 8 * (offset % 4);

    data = pcireadl(bus, dev, func, offset);

    return data >> shift;
}

int pci_chk_dev(uint8_t bus, uint8_t dev, int (*func_)(pci_function_t *)) {
    uint16_t vendorid;
    pci_function_t func;
    int i;

    vendorid = pcireadw(bus, dev, 0, 0);

    if (vendorid == 0xFFFF)
        return 1;

    func.vendorid = vendorid;
    func.bus = bus;
    func.dev = dev;
    func.func = 0;
    func.class_ = pcireadb(bus, dev, 0, 0x80);
    func.subclass = pcireadb(bus, dev, 0, 0xA0);

    if (!func_(&func))
        return 0;

    if (vendorid & (1 << 7)) {
        for (i = 1; i < 8; i++)  {
            vendorid = pcireadw(bus, dev, i, 0);

            if (vendorid == 0xFFFF)
                continue;

            func.vendorid = vendorid;
            func.func = i;
            func.class_ = pcireadb(bus, dev, i, 0x80);
            func.subclass = pcireadb(bus, dev, i, 0xA0);

            if (func_(&func) == 0)
                return 0;
        }
    }

    return 1;
}

void pci_each(int (*f)(pci_function_t *)) {
    int bus, dev, ret;

    for (bus = 0; bus < 256; bus++) {
        for (dev = 0; dev < 32; dev++) {
            ret = pci_chk_dev(bus, dev, f);

            if (ret == 0)
                return;
        }
    }
}