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

    uint8_t _class;
    uint8_t subclass;
    uint8_t prog_if;
    uint8_t rev_id;
    uint8_t head_type;
    uint8_t n_functions;
    bool multi_func;
} pci_header_t;

#endif