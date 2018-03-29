#include <kernel/timeout.h>
#include <kernel/drivers/cmos.h>
#include <stdbool.h>
#include <stdio.h>

static cmos_ts_t timestamp = 0;
static int until = 0;

void timeout_start(int seconds) {
    until = seconds;
    cmos_td_t timedate;
    timedate = rtc_gettime();
    timestamp = CMOS_SMALL_TS(timedate);
}

void timeout_wait(int (*check)(void), void (*failure)(void)) {
    cmos_td_t timedate = rtc_gettime();

    printf("until: %d\n", timestamp+until);
    while (1) {
        if (check()) {
            break;
        }
        timedate = rtc_gettime();
        if (CMOS_SMALL_TS(timedate) == timestamp+until)
            failure();
    }
}