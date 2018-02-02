#ifndef SATA_H
#define SATA_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define ATA_WRITE_DMA_EXT 0x35
#define ATA_WRITE_DMA     0xCA
#define ATA_READ_DMA_EXT  0x25
#define ATA_READ_DMA      0xC8
#define ATA_PACKET        0xA0

#define ATA_CB_DH_LBA     0x40

//ahci flags
#define AHCI_ATAPI 0x01
#define AHCI_WRITE 0x02

typedef struct {
    size_t lba; //just in case; could be used with a uint i'm pretty sure.
    unsigned scount;
} sata_op_t;

typedef struct {
    uint8_t reg;
    uint8_t cmd;
    uint8_t pmp_type;
    
    unsigned feat;

    unsigned scount;

    uint8_t ctrl;

    uint8_t res1;    
    uint8_t res2[48];    

    uint8_t lba_low;
    uint8_t lba_mid;
    uint8_t lba_high;

    uint8_t dev;
} ahci_fis_t;

typedef struct {
    ahci_fis_t fis;
    uint8_t    res[32];
    uint8_t    atapi[32];

    struct {
        uint32_t base;
        uint32_t baseu;
        uint32_t res;
        uint32_t flags;
    } prdt;
} ahci_cmd_t;

typedef struct {
    uint8_t  irq;
    void    *io_base;
    uint32_t caps;
    uint32_t ports;
} ahci_ctrl_t;

typedef struct {
    uint32_t flags;
    uint32_t bytes;
    uint32_t base;
    uint32_t baseu;
    uint32_t res[4];
} ahci_list_t;

typedef struct {
    unsigned scount;
} drive_t;

typedef struct {
    drive_t drive;
    ahci_ctrl_t *ctrl;
    ahci_list_t *list;
    ahci_fis_t  *fis;
    ahci_cmd_t  *cmd;
    uint32_t     pnr;
    uint32_t     atapi;
    char        *desc;
    int          prio;
} ahci_port_t;

#endif