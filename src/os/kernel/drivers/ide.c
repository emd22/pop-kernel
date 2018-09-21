#include <kernel/drivers/ide.h>
#include <kernel/drivers/pci.h>
#include <kernel/arch_io.h>

static ide_drive_t ide_drives[4] = {0, 0, 0, 0};

ide_drive_t *ide_drives_find(void) {
    return ide_drives;
}