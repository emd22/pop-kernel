#ifndef CMOS_H
#define CMOS_H

#include <stdint.h>

#define CMOS_SMALL_TS(c_td) ((c_td.hour*10000)+(c_td.minute*100)+c_td.second)

typedef struct {
    uint8_t century;
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    unsigned year;
} cmos_td_t;

typedef long cmos_ts_t;

cmos_td_t rtc_gettime();

#endif