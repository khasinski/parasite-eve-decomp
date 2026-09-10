#include "pe1/psyq_cd.h"

extern int D_800A3478, D_800A347C;
extern char *D_800A3480;
extern char D_80011C20[], D_80011B18[], D_80011B28[];
extern u8 D_8009AFD5;
extern char *D_8009AFDC[], *D_8009B05C[];
extern volatile u32 *D_8009B2C0;
int puts(const char *);
int printf(const char *, ...);
int CD_flush(void);

int CD_datasync(int mode) {
    int timeout_limit;
    char **commands;
    CdInterruptEvents *interrupts;
    char **events;

    D_800A3478 = VSync(-1) + 0x3C0;
    timeout_limit = 0x3C0000;
    commands = D_8009AFDC;
    interrupts = &D_8009B294;
    events = D_8009B05C;
    D_800A347C = 0;
    D_800A3480 = D_80011C20;
    do {
        if (VSync(-1) > D_800A3478 || D_800A347C++ > timeout_limit) {
            puts(D_80011B18);
            printf(D_80011B28, D_800A3480, commands[D_8009AFD5],
                   events[interrupts->sync], events[interrupts->ready]);
            CD_flush();
            return -1;
        }
        if (!(*D_8009B2C0 & 0x01000000)) {
            return 0;
        }
    } while (!mode);
    return 1;
}
