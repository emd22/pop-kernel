#ifndef TIMEOUT_H
#define TIMEOUT_H

void timeout_start(int seconds);
int timeout_tick();
void timeout_wait(int (*check)(void), void (*failure)(void));

#endif