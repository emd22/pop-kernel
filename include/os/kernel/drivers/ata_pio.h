#ifndef ATA_PIO_H
#define ATA_PIO_H

#include <stddef.h>
#include <stdint.h>

void ata_pio_install(void);
int ata_pio_read(size_t lba, uint8_t *buf);
void ata_pio_write(size_t lba, const uint8_t *buf, size_t d_len);
void wait_400ns(void);

#endif