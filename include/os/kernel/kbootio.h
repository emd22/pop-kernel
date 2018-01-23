#ifndef KBOOTIO_H
#define KBOOTIO_H

#define KINP_NOECHO 0x01

void kinp(char *buf, int flags);
void kinp_clr(void);

#endif