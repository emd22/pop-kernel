#ifndef ATA_PIO_H
#define ATA_PIO_H

#include <stddef.h>
#include <stdint.h>

#define READ 0x20 
#define WRITE 0x30

void ata_pio_install(void);
void ata_pio_read(unsigned addr, uint8_t *buf);
void ata_pio_write(unsigned addr, const uint8_t *buf);

#endif