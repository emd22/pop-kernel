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

void sata_init(void) {
    FIS_REG_H2D fis;
    bzero(&fis, sizeof(FIS_REG_H2D));

    fis.fis_type = FIS_TYPE_REG_H2D;
    fis.command  = FIS_CMD_IDENTIFY;
    fis.device   = 0; //master
    fis.c        = 1; //write command register
}

static int check_type(HBA_PORT *port) {
    uint32_t ssts = port->ssts;

    uint8_t ipm = (ssts >> 8) & 0x0F;
    uint8_t det = ssts & 0x0F;

    if (det != HBA_PORT_DET_PRESENT)
        return AHCI_DEV_NULL;
    if (ipm != HBA_PORT_IPM_ACTIVE)
        return AHCI_DEV_NULL;

    switch (port->sig) {
        case SATA_SIG_ATAPI:
            return AHCI_DEV_SATAPI;
        case SATA_SIG_SEMB:
            return AHCI_DEV_SEMB;
        case SATA_SIG_PM:
            return AHCI_DEV_PM;
        default:
            return AHCI_DEV_SATA;
    }
}

void probe_port(HBA_MEM *abar) {
    uint32_t pi = abar->pi;
    int i = 0;

    while (i < 32) {
        if (pi & 1) {
            int dt = check_type(&abar->ports[i]);
            if (dt == AHCI_DEV_SATA)
                printf("SATA drive found at port %d\n", i);
            else if (dt == AHCI_DEV_SATAPI)
                printf("SATAPI drive found at port %d\n", i);
            else if (dt == AHCI_DEV_SEMB)
                printf("SEMB drive found at port %d\n", i);
            else if (dt == AHCI_DEV_PM)
                printf("PM drive found at port %d\n", i);
            else
                printf("No drive found at port %d\n", i);
        }
        pi >>= 1;
        i++;
    }
}

void start_cmd(HBA_PORT *port) {
    while (port->cmd & HBA_PxCMD_CR);

    //set FRE(bit ) and ST(bit0)
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;
}

void stop_cmd(HBA_PORT *port) {
    port->cmd &= ~HBA_PxCMD_ST;

    while (1) {
        if (port->cmd & HBA_PxCMD_FR)
            continue;
        if (port->cmd & HBA_PxCMD_CR)
            continue;
        break;
    }

    //clear FRE
    port->cmd &= ~HBA_PxCMD_FRE;
}

void port_rebase(HBA_PORT *port, int portno) {
    //stop command engine
    stop_cmd(port);

    // Command list offset: 1K*portno
    // Command list entry size = 32
    // Command list entry maxim count = 32
    // Command list maxim size = 32*32 = 1K per port
    port->clb = AHCI_BASE + (portno << 10);
    port->clbu = 0;
    bzero((void *)(port->clb), 1024);

    port->fb = AHCI_BASE + (32 << 10) + (portno << 8);
    port->fbu = 0;
    bzero((void *)(port->fb), 256);

    HBA_CMD_HEADER *cmd_header = (HBA_CMD_HEADER *)(port->clb);

    int i;
    for (i = 0; i < 32; i++) {
        cmd_header[i].prdtl = 8; //8 prdt entries per command table
                                 //256 bytes per command table, 64+16+48+16*8
        //command table offset: 40K + 8K*portno + cmdheader_index*256
        cmd_header[i].ctba = AHCI_BASE + (40 << 10) + (portno << 13) + (i << 8);
        cmd_header[i].ctbau = 0;

        bzero((void *)cmd_header[i].ctba, 256);
    }
    start_cmd(port);
}