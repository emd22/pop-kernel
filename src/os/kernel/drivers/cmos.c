#include <kernel/drivers/cmos.h>
#include <kernel/drivers/idt.h>
#include <kernel/x86.h>

static int creg = 0x00; //century register


// void cmos_read(uint8_t *buf) {
//     int tval, index;

//     for (index = 0; index < 128; index++) {
//         _asm {
//             cli //disable interrupts
//             mov al, index
//             out 0x70, al
//             in al, 0x71
//             sti
//             mov tval, al
//         }
//         buf[index] = tval;
//     }
// }

// void cmos_write(uint8_t *buf) {
//     int tval, index;

//     for (index = 0; index < 128; index++) {
//         tval = buf[index];

//         _asm {
//             cli
//             mov al, index
//             out 0x70, al
//             mov al, tval
//             out 0x71, al
//             sti
//         }
//     }
// }

int check_updating() {
    outb(0x70, 0x0A);
    return (inb(0x71) & 0x80);
}

uint8_t get_reg(int reg) {
    outb(0x70, reg);
    return inb(0x71);
}

cmos_td_t rtc_gettime() {
    while (check_updating());
    cmos_td_t last;
    cmos_td_t cur;

    cur.second  = get_reg(0x00);
    cur.minute  = get_reg(0x02);
    cur.hour    = get_reg(0x04);
    cur.day     = get_reg(0x07);
    cur.month   = get_reg(0x08);
    cur.year    = get_reg(0x09);
    if (creg != 0x00) {
        cur.century = get_reg(creg);
    }

    int i;

    do {
        last = cur;
        
        if (creg != 0x00) {
            cur.century = get_reg(creg);
        }
    } while ((last.second != cur.second) || (last.minute != cur.minute) || (last.hour != cur.hour) ||
             (last.day != cur.day) || (last.month != cur.month) || (last.year != cur.year) ||
             (last.century != cur.century));

    uint8_t regb;
    regb = get_reg(0x0B);

    if ((regb & 0x04) == 0) {
        cur.second = (cur.second & 0x0F) + ((cur.second / 16) * 10);
        cur.minute = (cur.minute & 0x0F) + ((cur.minute / 16) * 10);
        cur.hour = ( (cur.hour & 0x0F) + (((cur.hour & 0x70) / 16) * 10) ) | (cur.hour & 0x80);
        cur.day = (cur.day & 0x0F) + ((cur.day / 16) * 10);
        cur.month = (cur.month & 0x0F) + ((cur.month / 16) * 10);
        cur.year = (cur.year & 0x0F) + ((cur.year / 16) * 10);
        if (creg != 0) {
                cur.century = (cur.century & 0x0F) + ((cur.century / 16) * 10);
        }
    }
    //convert 12h clock to 24h if necessary
    if (!(regb & 0x02) && (cur.hour & 0x80)) {
        cur.hour = ((cur.hour & 0x7F) + 12) % 24;
    }

    return cur;
}