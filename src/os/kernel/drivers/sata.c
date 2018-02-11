#include <kernel/drivers/sata.h>
#include <string.h>
#include <stdbool.h>
#include <osutil.h>

#include <kernel/err.h>

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

#define HBA_Px 0x0001
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_FR 0x4000
#define HBA_PxCMD_CR 0x8000

#define AHCI_PHYS_BASE 0x800000
#define AHCI_VIRT_BASE 0xFFFFFFFF00800000

#define AHCI_DEV_BUSY 0x80
#define AHCI_DEV_DRQ  0x08

#define BAR0 0x10

#define GB (1024l*1024l*1024l)
#define MB (1024l*1024)

#define PRDSIZE (4*MB)

#define PORT_CMD_ST  (1 << 0)   // start processing the command list
#define PORT_CMD_FRE (1 << 4)   // fis receive enable
#define PORT_CMD_FR  (1 << 14)  // fis receive running
#define PORT_CMD_CR  (1 << 15)  // command list running

#define PORT_IS_TFES (1 << 30)

HBA_CMD_HEADER cmdlist[32] __attribute__((aligned(1*1024)/* 1 KiB */));
HBA_CMD_TBL    cmdtbls[32] __attribute__((aligned(128)));
HBA_FIS        fisstorage  __attribute__((aligned(256)));

struct {
    bool found;
    HBA_MEM *abar;
} ahci;

struct {
    pci_function_t pci;
    HBA_MEM *base;
    int n_ports;
    int n_slots;
    int dma64;
} hba;

void ahci_init(void) {
    bzero(&hba, sizeof(hba));
    bzero(&ahci, sizeof(ahci));

    ahci.found = false;
    //...
}

bool find_ahci(pci_function_t *func) {
    if (func->class_ == PCI_C_STORAGE && func->subclass == PCI_SC_AHCI) {
        ahci.found = true;
        hba.pci = *func;
        return false;
    }
    else {
        return true;
    }
}

int pop_count(size_t x) {
    int count;
    for (count = 0; x; count++) {
        x &= x - 1; // clear least significant non-zero bit
    }
    return count;
}

unsigned get_pci_bar(pci_function_t *func, uint8_t bar) {
    return pcireadl(func->bus, func->dev, func->func, BAR0+4*bar);
}

HBA_MEM *get_abar(pci_function_t *func) {
    return (HBA_MEM *)(unsigned long)(get_pci_bar(func, 5) & 0xFFFFFFF0);
}

void checkalign(void *a, int alignment, char *msg) {
    uintptr_t aa = (uintptr_t)a;

    assert(!(aa & (alignment-1)), msg, NULL);
}

void portstart(HBA_PORT *port) {
    while (port->cmd & PORT_CMD_CR);

    port->cmd |= PORT_CMD_FRE;
    port->cmd |= PORT_CMD_ST;
}

void portstop(HBA_PORT *port) {
    port->cmd &= ~PORT_CMD_ST;
    port->cmd &= ~PORT_CMD_FRE;

    while (port->cmd & (PORT_CMD_FR | PORT_CMD_CR));
}

void portinit(HBA_PORT *port, HBA_CMD_HEADER *cl, HBA_CMD_TBL *ctlist, HBA_FIS *fisbase) {
    uintptr_t cll, fisbasel, ctlistl;
    int i;

    checkalign(cl, 1*1024, "portinit - cl align");
    checkalign((void *)fisbase, 256, "portinit - fisbase align");

    cll = (uintptr_t)cl;
    fisbasel = (uintptr_t)fisbase;
    ctlistl = (uintptr_t)ctlist;

    // assert(hba.dma64 && cll >= 4*GB, "[AHCI]: portinit - cl", NULL);
    // assert(hba.dma64 && fisbasel >= 4*GB, "[AHCI]: portinit - fisbase", NULL);
    // assert(hba.dma64 && ctlistl >= 4*GB, "[AHCI]: portinit - ctlist", NULL);

    portstop(port);

    port->clb = (unsigned)cll;
    if (hba.dma64)
        port->clbu = (unsigned)(cll >> 32);

    port->fb = (unsigned)fisbasel;
    if (hba.dma64)
        port->fbu = (unsigned)(fisbasel >> 32);

    for (i = 0; i < 32; i++) {
        cl[i].prdtl = 8;
        cl[i].ctba = (unsigned)ctlistl;
        if (hba.dma64)
            cl[i].ctbau = (unsigned)(ctlistl >> 32);
    }

    portstart(port);
}

bool ahci_detect(void) {
    HBA_MEM *base;

    pci_each(find_ahci);

    if (ahci.found) {
        base = hba.base = get_abar(&hba.pci);
        hba.n_ports = pop_count(base->pi);
        hba.n_slots = ((base->cap >> 8) & 0x1F) + 1;
        hba.dma64  = base->cap & (1 << 31);

        portinit(&base->ports[0], cmdlist, cmdtbls, &fisstorage);
    }

    return ahci.found;
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

// HBA_CMD_TBL *getcmdtbl(HBA_CMD_HEADER *cmdhdr) {
//     return (HBA_CMD_TBL *)(((size_t)cmdhdr->ctbau << 32) + cmdhdr->ctba);
// }

int find_cmdslot(HBA_PORT *port) {
    uint32_t slots = (port->sact | port->ci);
    unsigned slots_len = (ahci.abar->cap & 0x0F00) >> 8;

    int i;
    for (i = 0; i < slots_len; i++) {
        if (!(slots & 1))
            return i;
        slots >>= 1;
    }

    printf("AHCI: Cannot find command list entry. [%d]\n", i);
    return -1;
}

void start_cmd(HBA_PORT *port) {
    while (port->cmd & HBA_PxCMD_CR);

    //set FRE(bit4) and ST(bit0)
    port->cmd |= PORT_CMD_FRE;
    port->cmd |= PORT_CMD_ST;
}

void stop_cmd(HBA_PORT *port) {
    port->cmd &= ~PORT_CMD_ST;
    port->cmd &= ~PORT_CMD_FRE;

    while (true) {
        if (port->cmd & PORT_CMD_FR)
            continue;
        if (port->cmd & PORT_CMD_CR)
            continue;
        break;
    }

    //clear FRE
    port->cmd &= ~PORT_CMD_FRE;
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

void probe_port(HBA_MEM *abar_) {
    //Search disk in implemented ports
    uint32_t pi = abar_->pi;
    int i = 0;
    while (i < 32) {
        if (pi & 1) {
            int dt = check_type((HBA_PORT *)&abar_->ports[i]);

            if (dt == AHCI_DEV_SATA) {
                printf("SATA drive found at port %d\n", i);
                ahci.abar = abar_;
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

bool wait_busy(HBA_PORT *port, int *spin) {
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && (*spin) < 1000000) {
        (*spin)++;
    }
    if (*spin >= 1000000) {
        return AHCI_PORT_HUNG;
    }
    return true;
}

HBA_PORT *get_port(void) {
    return &hba.base->ports[0];
}

// HBA_CMD_HEADER *getcmdlist(HBA_PORT *port) {
//     return (HBA_CMD_HEADER *)(((size_t)port->clbu << 32) + port->clb);
// }

// void mkprd(HBA_PRDT_ENTRY *prd, uintptr_t addr, unsigned bytes) {
//     assert(bytes <= 4*MB, "[AHCI]: mkprd", NULL);

//     prd->dba = (unsigned)addr;
//     if (hba.dma64) {
//         prd->dbau = (unsigned)(addr >> 32);
//     }

//     prd->dbc = bytes - 1; // zero indexed
//     prd->i = 1;
// }

bool read(HBA_PORT *port, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf) {
	port->is = (uint32_t) -1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	if (slot == -1)
		return false;
 
	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER *)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (uint16_t)((count-1) >> 4)+1;	// PRDT entries count
 
	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL *)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) +
 		(cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
 
    int i;

	// 8K bytes (16 sectors) per PRDT
	for (i = 0; i < cmdheader->prdtl-1; i++)
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
		return AHCI_PORT_HUNG;
	}
 
	port->ci = 1<<slot;	// Issue command
 
	// Wait for completion
	while (true)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is & HBA_PxIS_TFES) { //task file error
			return AHCI_READ_ERR;
		}
	}
 
	// Check again
	if (port->is & HBA_PxIS_TFES)
	{
		return AHCI_READ_ERR;
	}
 
	return true;
}