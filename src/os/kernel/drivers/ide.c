#include <kernel/drivers/pata.h>
#include <kernel/drivers/pci.h>

#define ATA_SR_BSY  0x80
#define ATA_SR_DRDY 0x40
#define ATA_SR_DF   0x20
#define ATA_SR_DSC  0x10
#define ATA_SR_DRQ  0x08
#define ATA_SR_CORR 0x04
#define ATA_SR_IDX  0x02
#define ATA_SR_ERR  0x01

#define ATA_ER_BBK   0x80
#define ATA_ER_UNC   0x40
#define ATA_ER_MC    0x20
#define ATA_ER_IDNF  0x10
#define ATA_ER_MCR   0x08
#define ATA_ER_ABRT  0x04
#define ATA_ER_TK0NF 0x02
#define ATA_ER_AMNF  0x01

#define ATA_CMD_READ_PIO        0x20
#define ATA_CMD_READ_PIO_EXT    0x24
#define ATA_CMD_READ_DMA        0xC8
#define ATA_CMD_READ_DMA_EXT    0x25
#define ATA_CMD_WRITE_PIO       0x30
#define ATA_CMD_WRITE_PIO_EXT   0x34
#define ATA_CMD_WRITE_DMA       0xCA
#define ATA_CMD_WRITE_DMA_EXT   0x35
#define ATA_CMD_CACHE_FLUSH     0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET          0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY        0xEC

#define ATAPI_CMD_READ  0xA8
#define ATAPI_CMD_EJECT 0x1B

#define ATA_MASTER 0x00
#define ATA_SLAVE  0x01

#define ATA_PRIMARY   0x00
#define ATA_SECONDARY 0x01

#define IDE_ATA   0x00
#define IDE_ATAPI 0x01

#define ATA_REG_DATA         0x00
#define ATA_REG_ERROR        0x01
#define ATA_REG_FEATURES     0x01
#define ATA_REG_SECCOUNT0    0x02
#define ATA_REG_LBA0         0x03
#define ATA_REG_LBA1         0x04
#define ATA_REG_LBA2         0x05
#define ATA_REG_DRIVE_SELECT 0x06
#define ATA_REG_COMMAND      0x07
#define ATA_REG_STATUS       0x07
#define ATA_REG_SECCOUNT1    0x08
#define ATA_REG_LBA3         0x09
#define ATA_REG_LBA4         0x0A
#define ATA_REG_LBA5         0x0B
#define ATA_REG_CONTROL      0x0C
#define ATA_REG_ALTSTATUS    0x0C
#define ATA_REG_DEVADDRESS   0x0D

#define ATA_DRV_DEFAULT 0xA0
#define ATA_DRV_LBA     0x40
#define ATA_DRV_SLAVE   0x10

#define ATA_DRV_NULL   0x0000
#define ATA_DRV_PATA   0x0001
#define ATA_DRV_SATA   0xC33C
#define ATA_DRV_PATAPI 0xEB14
#define ATA_DRV_SATAPI 0x9669

struct ide_channel_regs {
    uint16_t base;
    uint16_t control;
    uint16_t bus_master_ide;
    uint8_t no_interrupt;
} channels[2];

struct ide_device {
    uint8_t exists;
    uint8_t channel; //primary or secondary channel(0 or 1)
    uint8_t drive; //master or slave (0 or 1)
    uint8_t type; //ata or atapi (0, 1)
    uint16_t signature;
    uint16_t features;
    unsigned command_sets;
    unsigned size; //size in sectors
    uint8_t model[41];
} ide_devices[4];

uint8_t ide_buffer[2048] = { 0 };
static uint8_t irq_invoked = 0;
static uint8_t atapi_packet[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static uint8_t bus_position = ATA_MASTER;

void wait_400ns(unsigned bar0) {
    inb(ATA_REG_STATUS);
    inb(ATA_REG_STATUS);
    inb(ATA_REG_STATUS);
    inb(ATA_REG_STATUS);
}

void ide_select_drive(uint8_t lba_high, uint8_t set_lba) {
    if (set_lba) {
        outb(ATA_REG_DRIVE_SELECT, ATA_DRV_DEFAULT | ATA_DRV_LBA | (bus_position == ATA_SLAVE ? ATA_SLAVE : ATA_MASTER); | lba_high);
    }
    else {
        outb(ATA_REG_DRIVE_SELECT, ATA_DRV_DEFAULT | (bus_position == ATA_SLAVE ? ATA_SLAVE : ATA_PRIMARY) | lba_high);
    }
}

int ide_send_command(uint8_t command) {
    outb(ATA_REG_COMMAND, command);
    int status, timeout = 20000000;
    do {
        wait_400ns();
        status = inb(ATA_REG_CONTROL);
    } while ((status & ATA_SR_BSY) != 0 && (status & ATA_SR_ERR) && timeout-- > 0);

    if (!(status & ATA_SR_ERR) || timeout == 0)
        return 1;
    
    return 0;
}

int ide_drive_discover() {
    ide_select_drive(0, 0);
    outb(ATA_REG_SECCOUNT0, 0);
    outb(ATA_REG_LBA0, 0);
    outb(ATA_REG_LBA1, 0);
    outb(ATA_REG_LBA2, 0);

    int status = ide_send_command(ATA_CMD_IDENTIFY);

    if (status & ATA_SR_ERR)
        return ATA_DRV_NULL;

    uint16_t type_id;
    type_id = (inb(ATA_REG_LBA2) << 8 | inb(ATA_REG_LBA1));

    if (type_id == ATA_DRV_SATA ||
        type_id == ATA_DRV_SATAPI ||
        type_id == ATA_DRV_PATAPI)
    {
        return type_id;
    }
}


void ide_init(unsigned bar0, unsigned bar1, unsigned bar2, unsigned bar3, unsigned bar4) {
    
}