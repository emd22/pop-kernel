#include <kernel/drivers/ahci.h>
#include <kernel/x86.h>

#include <kernel/timeout.h>

#define HBA_PORT_DET_PRESENT 3
#define HBA_PORT_IPM_ACTIVE  1

#define AHCI_DEV_NULL   0
#define AHCI_DEV_SATA   1
#define AHCI_DEV_SEMB   2
#define AHCI_DEV_PM     3
#define AHCI_DEV_SATAPI 4

#define	AHCI_BASE       0x400000 // 4M
 
#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000

#define	SATfA_SIG_ATA    0x00000101	// SATA drive
#define	SATA_SIG_ATAPI  0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB   0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	    0x96690101

HBA_MEM *abar;

void ahci_init() {

}

int ahci_check_type(HBA_PORT *port) {
    uint32_t ssts = port->ssts;

    uint8_t ipm = (ssts >> 8) & 0x0F;
    uint8_t det = ssts & 0x0F;

    printf("ipm: %d, sig: 0x%x(%d), det: %d\n", ipm, port->sig, port->sig, det);

    if (det != HBA_PORT_DET_PRESENT) {
        // printf("AHCI HBA port not present. %d\n", det);
        return AHCI_DEV_NULL;
    }

    if (ipm != HBA_PORT_IPM_ACTIVE) {
        return AHCI_DEV_NULL;
    }

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

void ahci_probe_port(HBA_MEM *abar_) {
    abar = abar_;

    uint32_t pi = abar->pi;
    int i = 0;

    while (i < 32) {
        if (pi & 1) {
            int dt = ahci_check_type(&abar->ports[i]);
            if (dt == AHCI_DEV_SATA) {
                printf("SATA drive found at port %d\n", i);
                ahci_port_rebase(abar->ports, i);
            }
            else if (dt == AHCI_DEV_SATAPI)
                printf("SATAPI drive found at port %d\n", i);
            else if (dt == AHCI_DEV_SEMB)
                printf("SEMB drive found at port %d\n", i);
            else if (dt == AHCI_DEV_PM)
                printf("PM drive found at port %d\n", i);
            // else
            //     printf("No drive found at port %d -> %d\n", i, dt);
        }

        pi >>= 1;
        i++;
    }
}

void ahci_start_command_system(HBA_PORT *port) {
    while (port->cmd & HBA_PxCMD_CR);

    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;
}

void ahci_stop_command_system(HBA_PORT *port) {
    port->cmd &= ~HBA_PxCMD_ST;
    
    while (1) {
        if (port->cmd & HBA_PxCMD_FR)
            continue;
        if (port->cmd & HBA_PxCMD_CR)
            continue;
        break;
    }

    port->cmd &= ~HBA_PxCMD_FRE;
}

void ahci_port_rebase(HBA_PORT *port, int portno) {
    ahci_stop_command_system(port);

    port->clb = AHCI_BASE + (portno << 10);
    port->clbu = 0;
    memset((void *)(port->clb), 0, 1024);

    port->fb = AHCI_BASE+(32 << 10)+(portno << 8);
    port->fbu = 0;
    memset((void *)(port->fb), 0, 256);

    HBA_CMD_HEADER *cmd_header = (HBA_CMD_HEADER *)(port->clb);

    int i;
    for (i = 0; i < 32; i++) {
        cmd_header[i].prdtl = 8;
        cmd_header[i].ctba = AHCI_BASE+(40 << 10)+(portno << 13)+(i << 8);
        printf("cmdheader[%d].ctba=%d\n", i, cmd_header[i].ctba);
        cmd_header[i].ctbau = 0;
        memset((void *)cmd_header[i].ctba, 0, 256);
    }

    ahci_start_command_system(port);
}

int find_cmdslot(HBA_PORT *port) {
    // If not set in SACT and CI, the slot is free
    uint32_t slots = (port->sact | port->ci);
    int i;

    // int cmdslots = abar->cap & (0x1F);
    int cmdslots = 32;
    // printf("cmdslots -> %d\n", cmdslots);

    for (i = 0; i < cmdslots; i++) {
        if ((slots & 1) == 0)
            return i;
        slots >>= 1;
    }
    printf("Cannot find free command list entry\n");
    return -1;
}

bool read(HBA_PORT *port, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf) {
    port->is = (uint32_t) -1;		// Clear pending interrupt bits
    int spin = 0; // Spin lock timeout counter
    int slot = find_cmdslot(port);
    if (slot == -1)
        return false;    
 
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)port->clb;
    cmdheader += slot;
    cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);	// Command FIS size
    cmdheader->w = 0;		// Read from device
    cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;	// PRDT entries count

    // printf("");
    printf("ptr: %d\n", cmdheader->ctba);
    while(1);
    HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);
    memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) +
         (cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
 
    // 8K bytes (16 sectors) per PRDT
    int i;
    for (i=0; i<cmdheader->prdtl-1; i++)
    {
        cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
        cmdtbl->prdt_entry[i].dbc = 8*1024-1;	// 8K bytes (this value should always be set to 1 less than the actual value)
        cmdtbl->prdt_entry[i].i = 1;
        buf += 4*1024;	// 4K words
        count -= 16;	// 16 sectors
    }
    // Last entry
    cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
    cmdtbl->prdt_entry[i].dbc = (count<<9)-1;	// 512 bytes per sector
    cmdtbl->prdt_entry[i].i = 1;
 
    // Setup command
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
 
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;	// Command
    cmdfis->command = ATA_CMD_READ_DMA_EXT;
 
    cmdfis->lba0 = (uint8_t)startl;
    cmdfis->lba1 = (uint8_t)(startl>>8);
    cmdfis->lba2 = (uint8_t)(startl>>16);
    cmdfis->device = 1<<6;	// LBA mode
 
    cmdfis->lba3 = (uint8_t)(startl>>24);
    cmdfis->lba4 = (uint8_t)starth;
    cmdfis->lba5 = (uint8_t)(starth>>8);
 
    cmdfis->countl = count & 0xFF;
    cmdfis->counth = (count >> 8) & 0xFF;

 
    // The below loop waits until the port is no longer busy before issuing a new command
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
    {
        spin++;
    }
    if (spin == 1000000)
    {
        printf("Port is hung\n");
        return false;
    }
 
    port->ci = 1<<slot;	// Issue command
 
    // Wait for completion
    while (1)
    {
        // In some longer duration reads, it may be helpful to spin on the DPS bit 
        // in the PxIS port field as well (1 << 5)
        if ((port->ci & (1<<slot)) == 0) 
            break;
        if (port->is & HBA_PxIS_TFES)	// Task file error
        {
            printf("Read disk error\n");
            return false;
        }
    }
 
    // Check again
    if (port->is & HBA_PxIS_TFES)
    {
        printf("Read disk error\n");
        return false;
    }
 
    return true;
}