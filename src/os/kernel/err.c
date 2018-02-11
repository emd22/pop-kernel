#include <kernel/err.h>
#include <string.h>

#include <osutil.h>

const char *get_err(int err) {
    if (err & AHCI_READ_ERR)
        return "AHCI read error";
    else if (err & AHCI_WRITE_ERR)
        return "AHCI write error";
    else if (err & AHCI_PORT_HUNG)
        return "AHCI port hung";
    else
        return NULL;
}

void chk_err__(int res) {
    const char *res_ = get_err(res);

    if (res_ != NULL) {
        printf("CHK_ERR HIT: %s, HALTING...\n", res_);
        assert(0, "chk_err failed.", NULL);
    }
}