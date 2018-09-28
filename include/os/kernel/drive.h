#ifndef DRIVE_H
#define DRIVE_H

#include <stdint.h>
#include <stddef.h>

#define DRIVE_EXISTS 0x01

typedef struct {
    uint8_t bus;
    uint8_t bus_position;
    char serial_number[21];
    char firmware_revision[9];
    char model_number[41];
    size_t blocks;
    int flags;

    int controller_type;
    int controller_pci_index;

    void (*read_block)(unsigned, uint16_t, uint8_t *);
    void (*write_block)(unsigned, uint16_t, const uint8_t *);
} drive_t;

#endif