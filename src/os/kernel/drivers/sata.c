#include <kernel/drivers/sata.h>
#include <string.h>

#define FIS_CMD_IDENTIFY 0xEC

#define SATA_SIG_ATA   0x00000101 //sata
#define SATA_SIG_ATAPI 0xEB140101 //satapi
#define SATA_SIG_SEMB  0xC33C0101
#define SATA_SIG_PM    0x96690101

#define AHCI_DEV_NULL   0
#define AHCI_DEV_SATA   1
#define AHCI_DEV_SEMB   2
#define AHCI_DEV_PM     3
#define AHCI_DEV_SATAPI 4

#define HBA_PORT_IPM_ACTIVE  1
#define HBA_PORT_DET_PRESENT 3

#define AHCI_BASE 0x400000 //4M

#define HBA_PxCMD_ST 0x0001
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_FR 0x4000
#define HBA_PxCMD_CR 0x8000

#define PORT_TO_CTRL(port, reg) (0x100+((port*0x80)+reg))

// void sata_init_(FIS_REG_H2D *fis) {
//     memset(fis, 0, sizeof(*fis));
// }

void sata_prep_rw(ahci_fis_t *fis, sata_op_t *op, uint8_t is_write) {
    /*Logical block address; normally 512 bytes long.*/
    size_t lba = op->lba;
    uint8_t cmd = 0;

    memset(fis, 0, sizeof(*fis));

    if (op->scount >= (1 << 8) || lba+op->scount >= (1 << 28)) {
        lba &= 0xFFFFFF;
        cmd = (is_write ? ATA_WRITE_DMA_EXT :
                          ATA_READ_DMA_EXT);
    }
    else {
        cmd = (is_write ? ATA_WRITE_DMA :
                          ATA_READ_DMA);
    }

    fis->lba_low  = (uint8_t)lba;
    fis->lba_mid  = (uint8_t)(lba >> 8);
    fis->lba_high = (uint8_t)(lba >> 16);
    fis->feat     = 1; //dma
    fis->scount   = op->scount;
    fis->cmd      = cmd;
    fis->dev      = ((lba >> 24) & 0x40) | ATA_CB_DH_LBA;
}

void sata_prep_atapi(sata_fis_t *fis, uint16_t bsize) {
    memset_fl(fis, 0, sizeof(*fis));
    fis->lba_mid  = bsize;
    fis->lba_high = bsize >> 8;
    fis->feat     = 1; //atapi dma
    fis->cmd      = ATA_PACKET;
}

uint32_t port_readl(void *base, uint32_t port, uint32_t reg) {
    return readl(base+PORT_TO_CTRL(port, reg));
}

void port_writel(void *base, uint32_t port, uint32_t reg, uint32_t val) {
    writel(base+PORT_TO_CTRL(port, reg), val);
}

int ahci_command(ahci_port_t *port, void *buf, unsigned size, int flags) {
    ahci_ctrl_t *ctrl = port->ctrl;
    ahci_cmd_t  *cmd  = port->cmd;
    ahci_list_t *list = port->list;

    cmd->fis.reg = 0x27;
    cmd->fis.pmp_type = 1 << 7;

    cmd->prdt.base = (uint32_t)buf;
    cmd->prdt.baseu = 0;
    cmd->prdt.flags = size-1;

    uint32_t l_flags;
    l_flags = ((1 << 16) |
               (flags & AHCI_WRITE ? (1 << 6) : 0) |
               (flags & AHCI_ATAPI ? (1 << 5) : 0) | 5);

    list->flags  = l_flags;
    list->bytes  = 0;
    list->base   = (uint32_t)cmd;
    list->baseu  = 0;

    uint32_t bits = port_readl(ctrl->io_base, port->pnr);
}