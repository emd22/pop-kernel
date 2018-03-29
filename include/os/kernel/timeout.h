#ifndef TIMEOUT_H
#define TIMEOUT_H

void timeout_start(int seconds);
void timeout_wait(int (*check)(void), void (*failure)(void));

#endif