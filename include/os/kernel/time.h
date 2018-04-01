#ifndef TIME_H
#define TIME_H

#include <kernel/drivers/cmos.h>

#define TIME_ALL 0x01
#define TIME_HOUR 0x02
#define TIME_MINUTE 0x04
#define TIME_SECOND 0x08

#define TIME_SMALL_TS(c_td) ((c_td.hour*10000)+(c_td.minute*100)+c_td.second)

typedef unsigned long long time_t;
typedef long timestamp_t;

void time_normalize(cmos_td_t *ttime);
const char *time_day_full(int day);
const char *time_get_month(int month);
const char *time_str(cmos_td_t *td, int flags);

#endif