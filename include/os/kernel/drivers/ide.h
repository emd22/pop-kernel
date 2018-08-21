#ifndef IDE_H
#define IDE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    char serial_number[21];
    char firmware_revision[9];
    char model_number[41];
    int max_write_pio;
    int blocks;
    bool lba48;
} ide_drive_t;

void ide_read_block(size_t block_pos, unsigned block_count, char *data);
void ide_write_block(size_t block_pos, unsigned block_count, char *data);
int ide_init(void);

#endif