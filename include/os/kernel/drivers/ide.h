#ifndef IDE_H
#define IDE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <kernel/drive.h>

enum {
    IDE_DRV_EXISTS = 0x01,
    IDE_DRV_LBA48  = 0x02,
};

void ide_read_block(unsigned lba, uint16_t sector_count, uint8_t *data);
void ide_write_block(unsigned lba, uint16_t sector_count, const uint8_t *data);
void ide_set_bus(int _bus, int _bus_position);
void ide_init(drive_t *drive_buf, int *drive_index);

#endif