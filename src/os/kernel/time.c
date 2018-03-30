#include <kernel/time.h>
#include <string.h>
#include <stdio.h>

#define ADD_NEXT(n1, n2, max) { \
if (n1 > max) {                 \
    n2 += n1/max;               \
    n1 = n1 % max;              \
}                               \
}                               \

char *dec_to_str(unsigned x, char *s) {
    *--s = 0;
    if (!x) 
        *--s = '0';

    for (; x; x/=10) 
        *--s = '0'+x % 10;
        
    return s;
}

void time_normalize(cmos_td_t *ttime) {
    ADD_NEXT(ttime->second, ttime->minute, 59);
    ADD_NEXT(ttime->minute, ttime->hour,   59);
    ADD_NEXT(ttime->hour,   ttime->day,    24);
}

const char *time_day_full(int day) {
    static char msg[12];
    strcpy(msg, dec_to_str(day, msg));
    
    const char *end;

    if (day == 1)
        end = "st";
    else if (day == 2)
        end = "nd";
    else if (day == 3)
        end = "rd";
    else
        end = "th";

    strcat(msg, end);

    return msg;
}

const char *time_get_month(int month) {
    static const char months[][10] = {
        "January", "Febuary", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    return months[month];
}