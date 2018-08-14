#include <kernel/time.h>
#include <string.h>
#include <stdio.h>

#define ADD_NEXT(n1, n2, max) { \
if (n1 > max) {                 \
    n2 += n1-max;               \
    n1 -= max;                  \
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

    return msg;
}

const char *time_get_month(int month) {
    static const char months[][10] = {
        "January", "Febuary", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    return months[month-1];
}

const char *time_zero(int tm) {
    static char buf[4];
    if (tm < 10) {
        buf[0] = '0';
        /* strcat(buf,  */dec_to_str(tm, buf);/* ); */
    }
    else {
        strcpy(buf, dec_to_str(tm, buf));
    }
    return buf;
}

const char *time_str(cmos_td_t *td, int flags) {
    static char buf[32];

    if (flags & TIME_HOUR || flags & TIME_ALL)
        strcpy(buf, time_zero(td->hour));

    if ((flags & TIME_HOUR && flags & TIME_MINUTE) || flags & TIME_ALL)
        strcat(buf, ":");

    if (flags & TIME_MINUTE || flags & TIME_ALL)
        strcat(buf, time_zero(td->minute));

    if ((flags & TIME_MINUTE && flags & TIME_SECOND) || flags & TIME_ALL)
        strcat(buf, ":");
    
    if (flags & TIME_SECOND || flags & TIME_ALL)
        strcat(buf, time_zero(td->second));

    strcat(buf, "\0");
    
    return buf;
}