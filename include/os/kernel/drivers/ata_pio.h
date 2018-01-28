#ifndef ATA_PIO_H
#define ATA_PIO_H

#include <stddef.h>
#include <stdint.h>

#define READ 0x20 
#define WRITE 0x30

void ata_pio_install(void);
void ata_pio_read(uint16_t lba, void *buf, int sc);
void ata_pio_write(uint16_t lba, const uint8_t *buf, size_t dl);

#endif