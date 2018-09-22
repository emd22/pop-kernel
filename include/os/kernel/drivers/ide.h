#ifndef IDE_H
#define IDE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum {
    IDE_DRV_EXISTS = 0x01,
    IDE_DRV_LBA48  = 0x02,
};

typedef struct {
    uint8_t bus;
    uint8_t bus_position;
    char serial_number[21];
    char firmware_revision[9];
    char model_number[41];
    int max_write_pio;
    size_t blocks;
    int flags;
} ide_drive_t;

void ide_set_bus(uint8_t _bus, uint8_t _bus_position);
void ide_read_block(size_t block_pos, unsigned block_count, char *data);
void ide_write_block(unsigned lba, uint16_t sector_count, const uint8_t *data);
ide_drive_t *ide_init(void);

#endif