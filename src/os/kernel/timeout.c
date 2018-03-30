#include <kernel/timeout.h>
#include <kernel/time.h>
#include <stdbool.h>
#include <stdio.h>

static cmos_ts_t timestamp = 0;

void nop() {}

void timeout_start(int seconds) {
    cmos_td_t timedate;
    timedate = cmos_rtc_gettime();
    timedate.second += seconds;
    time_normalize(&timedate);
    timestamp = TIME_SMALL_TS(timedate);
}

int timeout_tick() {
    cmos_td_t timedate = cmos_rtc_gettime();
    if (TIME_SMALL_TS(timedate) >= timestamp) {
        return 1;
    }
    return 0;
}

void timeout_wait(int (*check)(void), void (*failure)(void)) {
    cmos_td_t timedate = cmos_rtc_gettime();
    int i;

    while (1) {
        for (i = 0; i < 10000; i++)
            nop();

        if (check()) {
            break;
        }
        timedate = cmos_rtc_gettime();
        if (TIME_SMALL_TS(timedate) == timestamp)
            failure();
    }
}