#ifndef PCI_H
#define PCI_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t bus;
    uint8_t slot;
    uint8_t function;

    uint16_t vend_id;
    uint16_t dev_id;

    uint8_t class_;
    uint8_t subclass;
    uint8_t prog_if;
    uint8_t rev_id;
    uint8_t head_type;
    uint8_t n_functions;
    bool multi_func;
} pci_header_t;

void pci_init(void);
uint16_t pci_config_readw(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t pci_check_vendor(uint8_t bus, uint8_t slot);
bool dev_is_valid(uint8_t bus, uint8_t slot, uint8_t func);
void create_dev_header(uint8_t bus, uint8_t slot, uint8_t func);
void set_hdr(int index, uint8_t bus, uint8_t slot, uint8_t func);
bool chk_multi_func(uint8_t bus, uint8_t slot);
void scan_brute_force(int (*cb)(pci_header_t *));
pci_header_t *find_dev(uint8_t bus, uint8_t slot, uint8_t func);

#endif