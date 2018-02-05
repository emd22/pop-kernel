#include <kernel/drivers/sata.h>
#include <string.h>
#include <stdbool.h>
#include <osutil.h>

/*
Some AHCI/SATA code taken from osdev.org.
*/

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

#define AHCI_PHYS_BASE 0x800000
#define AHCI_VIRT_BASE 0xFFFFFFFF00800000

#define MEM_KERN_BASE 0xFFFFFFFF80000000

#define AHCI_DEV_BUSY 0x80
#define AHCI_DEV_DRQ  0x08

HBA_MEM *abar;

void ahci_alloc_pages(uint32_t npages) {
    uint64_t vaddr, paddr;
    
    vaddr = AHCI_VIRT_BASE;
    paddr = AHCI_PHYS_BASE;

    int i;
    for (i = 0; i < npages; ++i) {
        
    }
}

void ahci_init(void) {
    uint64_t paddr = 0xFEBF0000;
    uint64_t vaddr = 0;

    
}

void probe_port(HBA_MEM *abar_) {
    //Search disk in implemented ports
    uint32_t pi = abar_->pi;
    int i = 0;
    while (i < 32) {
        if (pi & 1) {
            int dt = check_type((HBA_PORT *)&abar_->ports[i]);

            if (dt == AHCI_DEV_SATA) {
                printf("SATA drive found at port %d\n", i);
                abar = abar_;
                port_rebase(abar_->ports, i);
                //read(&abar_temp->ports[0], 0, 0, 2, (uint64_t)pages_for_ahci_start + (20*4096)/8);
                //print("\nafter read %d",((HBA_PORT *)&abar_temp->ports[i])->ssts);
                return;
            }
        }
        pi >>= 1;
        i++;
    }
}

int find_cmdslot(HBA_PORT *port) {
    uint32_t slots = (port->sact | port->ci);
    unsigned slots_len = (abar->cap & 0x0F00) >> 8;

    int i;
    for (i = 0; i < slots_len; i++) {
        if (!(slots & 1))
            return i;
        slots >>= 1;
    }

    printf("AHCI: Cannot find command list entry. [%d]\n", i);
    return -1;
}

int check_type(HBA_PORT *port) {
    uint32_t ssts = port->ssts;

    uint8_t ipm = (ssts >> 8) & 0x0F;
    uint8_t det = ssts & 0x0F;

    printf("ipm %d det %d sig %d\n", ipm, det, port->sig); 
    if (det != HBA_PORT_DET_PRESENT)    // Check drive status
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

    return AHCI_DEV_NULL;
}


void start_cmd(HBA_PORT *port) {
    while (port->cmd & HBA_PxCMD_CR);

    //set FRE(bit4) and ST(bit0)
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;
}

void stop_cmd(HBA_PORT *port) {
    port->cmd &= ~HBA_PxCMD_ST;
    port->cmd &= ~HBA_PxCMD_FRE;

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

void port_rebase(HBA_PORT *port, int port_no) {
    int i;
    uint64_t clb_addr, fbu_addr, ctb_addr;
    stop_cmd(port);

    port->clb  = (((uint64_t)AHCI_PHYS_BASE & 0xFFFFFFFF));
    port->clbu = 0;
    port->fb   = (((uint64_t)AHCI_PHYS_BASE + (uint64_t)((32 << 10))) & 0xFFFFFFFF);
    port->fbu  = ((((uint64_t)AHCI_PHYS_BASE + (uint64_t)((32 << 10))) >> 32) & 0xFFFFFFFF);

    port->serr = 1;
    port->is   = 0;
    port->ie   = 1;

    clb_addr = 0;
    clb_addr = (((clb_addr | port->clbu) << 32) | port->clb);
    clb_addr = clb_addr + MEM_KERN_BASE;
    memset((void *)clb_addr, 0, 1024);

    fbu_addr = 0;
    fbu_addr = (((fbu_addr | port->fbu) << 32) | port->fb);
    fbu_addr = fbu_addr + MEM_KERN_BASE;
    memset((void *)fbu_addr, 0, 256);

    clb_addr = 0;
    clb_addr = (((clb_addr | port->clbu) << 32) | port->clb);
    clb_addr = (clb_addr + MEM_KERN_BASE);

    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER *)clb_addr;
    for (i = 0; i < 32; i++) {
        cmdheader[i].prdtl = 8; // 8 prdt entries per command table
        // 256 bytes per command table, 64+16+48+16*8
        // Command table offset: 40K + 8K*portno + cmdheader_index*256
        cmdheader[i].ctba  = (((uint64_t)AHCI_PHYS_BASE + (uint64_t) ((40 << 10)) + (uint64_t)((i << 8))) & 0xFFFFFFFFF);
        cmdheader[i].ctbau = ((((uint64_t)AHCI_PHYS_BASE + (uint64_t) ((40 << 10)) + (uint64_t)((i << 8))) >> 32)& 0xFFFFFFFF);

        ctb_addr = 0;
        ctb_addr = (((ctb_addr | cmdheader[i].ctbau) << 32) | cmdheader[i].ctba);
        ctb_addr =  ctb_addr + MEM_KERN_BASE;

        memset((void *)ctb_addr, 0, 256);
    }

    start_cmd(port);

    port->is = 0;   
    port->ie = 0xFFFFFFFF;
}

bool wait_busy(HBA_PORT *port, int *spin) {
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && (*spin) < 1000000) {
        (*spin)++;
    }
    if (spin >= 1000000) {
        printf("[AHCI]: err: port hung\n");
        return false;
    }
    return true;
}

bool sata_read(HBA_PORT *port, uint32_t startl, uint32_t starth, uint32_t scount, uint16_t *buf) {
    /*Clear pending interrupt bits*/
    port->is = (uint32_t)-1;

    int slot = find_cmdslot(port);

    if (slot == -1)
        return false;
    
    HBA_CMD_HEADER *cmd_header;
    cmd_header = (HBA_CMD_HEADER *)port->clb;

    cmd_header += slot;
    cmd_header->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);
    cmd_header->w = 0;
    cmd_header->prdtl = (uint16_t)((scount-1) >> 4)+1;

    HBA_CMD_TBL *cmd_tbl = (HBA_CMD_TBL *)(cmd_header->ctba);
    memset(cmd_tbl, 0, sizeof(HBA_CMD_TBL)+(cmd_header->prdtl-1)*sizeof(HBA_PRDT_ENTRY));

    int i;
    HBA_PRDT_ENTRY *tprdt = NULL;

    for (i = 0; i < cmd_header->prdtl-1; i++) {
        tprdt = &(cmd_tbl->prdt_entry[i]);
        tprdt->dba = (uint32_t)buf;
        tprdt->dbc = 8*1024-1;
        tprdt->i = 1;
        buf += 4*1024; //4k words
        scount -= 16; //16 sectors
    }

    tprdt->dba = (uint32_t)buf;
    tprdt->dbc = (scount << 9)-1; //512 bytes per sector
    tprdt->i   = 1;

    /*Setup command*/
    FIS_REG_H2D *cmd_fis = (FIS_REG_H2D *)(&cmd_tbl->cfis);

    cmd_fis->fis_type = FIS_TYPE_REG_H2D;
    cmd_fis->c = 1;
    cmd_fis->command = ATA_CMD_READ_DMA_EXT;

    /*Get first byte of DWORD*/
    cmd_fis->lba0 = (uint8_t)startl;
    /*bitshift to get next byte*/
    cmd_fis->lba1 = (uint8_t)(startl >> 8);
    /*get next byte*/
    cmd_fis->lba2 = (uint8_t)(startl >> 16);
    
    cmd_fis->device = 1 << 6; //LBA mode, do not use CHS as it is really outdated.

    cmd_fis->lba3 = (uint8_t)(startl >> 24);
    cmd_fis->lba4 = (uint8_t)starth;
    cmd_fis->lba5 = (uint8_t)(starth >> 8);

    cmd_fis->countl = scount & 0xFF;
    cmd_fis->counth = (scount >> 8) & 0xFF;

    int spin = 0;

    if (!wait_busy(port, &spin))
        return false;

    port->ci = 1 << slot; //issue command

    while (true) { //wait for completion
        if (port->ci & (1 << slot) == 0)
            break;
        if (port->is & HBA_PxIS_TFES) {
            printf("[AHCI]: err: disk read error\n");
            return false;
        }
    }

    //check again

    if (port->is & HBA_PxIS_TFES) {
        printf("[AHCI]: err: disk read error[2]\n");
        return false;
    }
    return true;
}

bool sata_write(HBA_PORT *port, uint32_t startl, uint32_t starth, uint32_t scount, uint64_t *buf) {
    port->is = 0xFFFF; //clear pending interrupt bits

    int slot = find_cmdslot(port);
    if (slot == -1)
        return false;

    uint64_t clb = 0;
    clb = (((clb | port->clbu) << 32) | port->clb);

    HBA_CMD_HEADER *cmd_header = (HBA_CMD_HEADER *)(MEM_KERN_BASE+clb);

    cmd_header += slot;

    cmd_header->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);
    cmd_header->w = 1; //write to device
    cmd_header->c = 1;

    cmd_header->prdtl = 1; //prdt entries count

    uint64_t ctb = 0;
    ctb = (((ctb | cmd_header->ctbau) << 32) | cmd_header->ctba);

    HBA_CMD_TBL *cmd_tbl = (HBA_CMD_TBL *)(MEM_KERN_BASE+ctb);
    memset((uint64_t *)(MEM_KERN_BASE + ctb), 0, sizeof(HBA_CMD_TBL));


    HBA_PRDT_ENTRY *prdt = &(cmd_tbl->prdt_entry[0]);

    prdt->dba = (uint32_t)(buf & 0xFFFFFFFF);
    prdt->dbau = (uint32_t)((buf >> 32) & 0xFFFFFFFF);
    prdt->dbc = 4096-1; //512 bytes per sector
    prdt->i = 0;

    FIS_REG_H2D *cmd_fis = (FIS_REG_H2D *)(&cmd_tbl->cfis);

    cmd_fis->fis_type = FIS_TYPE_REG_H2D;
    cmd_fis->c = 1; //command
    cmd_fis->command = ATA_CMD_WRITE_DMA_EXT;

    cmd_fis->lba0 = (uint8_t)startl;
    cmd_fis->lba1 = (uint8_t)(startl >> 8);
    cmd_fis->lba2 = (uint8_t)(startl >> 16);
    cmd_fis->device = 1 << 6; //LBA mode

    cmd_fis->lba3 = (uint8_t)(startl >> 24);
    cmd_fis->lba4 = (uint8_t)starth;
    cmd_fis->lba5 = (uint8_t)(starth >> 8);

    cmd_fis->countl = 2;
    cmd_fis->counth = 0;

    port->ci = 1 << slot;

    while (true) {
        if (!(port->ci & (1 << slot)))
            break;
        if (port->is & HBA_PxIS_TFES) {
            printf("[AHCI]: err: disk write error\n");
            return false;
        }
    }

    if (port->is & HBA_PxIS_TFES) {
        printf("[AHCI]: err: disk write error[2]\n");
        return false;
    }
    return true;
}
