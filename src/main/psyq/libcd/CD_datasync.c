/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-expensive-optimizations */

#include "pe1/psyq_cd.h"

extern u8 D_8009AFD5;
extern int D_800A3478;
extern int D_800A347C;
extern char *D_800A3480;
extern char D_80011B18[];
extern char D_80011B28[];
extern char D_80011C20[];
extern char *D_8009AFDC[];
extern char *D_8009B05C[];
extern volatile u32 *D_8009B2C0;

int printf(const char *format, ...);
int puts(const char *text);
void CD_flush(void);

static inline int data_timed_out(char **commands, char **events, int limit,
                                 CdInterruptEvents *state) {
    if (VSync(-1) > D_800A3478 || D_800A347C++ > limit) {
        puts(D_80011B18);
        printf(D_80011B28, D_800A3480, commands[D_8009AFD5],
               events[state->sync], events[state->ready]);
        CD_flush();
        return -1;
    }
    return 0;
}

int CD_datasync(int mode) {
    int saved_mode = mode;
    int limit;
    char **commands;
    char **events;
    CdInterruptEvents *state;

    D_800A3478 = VSync(-1) + 0x3C0;
    D_800A347C = 0;
    D_800A3480 = D_80011C20;
    limit = 0x3C0000;
    commands = D_8009AFDC;
    state = &D_8009B294;
    events = D_8009B05C;

    do {
        if (data_timed_out(commands, events, limit, state)) {
            return -1;
        }
        if (!(*D_8009B2C0 & 0x1000000)) {
            return 0;
        }
    } while (!saved_mode);

    return 1;
}
