#ifndef ATA_PIO_H
#define ATA_PIO_H

#include <stddef.h>
#include <stdint.h>

void ata_pio_cache_flush(void);
void ata_pio_install(void);
void ata_pio_read(size_t lba, size_t buf_pos, uint8_t *buf, size_t sectors);
void ata_pio_write(size_t lba, uint8_t *data, size_t dat_len);

#endif