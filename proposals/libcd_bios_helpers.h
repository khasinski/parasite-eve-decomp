#ifndef PROPOSALS_LIBCD_BIOS_HELPERS_H
#define PROPOSALS_LIBCD_BIOS_HELPERS_H

#include "pe1/psyq_cd.h"

extern u8 D_8009AFD5;
extern int D_800A3478, D_800A347C;
extern char *D_800A3480;
extern char D_80011B18[], D_80011B28[];
int printf(const char *format, ...);
int puts(const char *text);
void CD_flush(void);

static inline int timed_out(char **commands, char **events) {
    if (VSync(-1) > D_800A3478 || D_800A347C++ > 0x3C0000) {
        puts(D_80011B18);
        printf(D_80011B28, D_800A3480, commands[D_8009AFD5],
               events[D_8009B294.sync], events[D_8009B294.ready]);
        CD_flush();
        return -1;
    }
    return 0;
}

static inline void copy_result(u8 *destination, const u8 *source) {
    int remaining;
    if (destination) {
        remaining = 7;
        do { *destination++ = *source++; } while (--remaining != -1);
    }
}

#endif
