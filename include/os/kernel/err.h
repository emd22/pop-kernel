#ifndef ERR_H
#define ERR_H

#define AHCI_READ_ERR 0x01
#define AHCI_WRITE_ERR 0x02
#define AHCI_PORT_HUNG 0x04

#define chk_err(func_, ...) chk_err__(func_(__VA_ARGS__))

void chk_err__(int res);

#endif