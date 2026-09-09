/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_cd.h"

extern int D_8009AFC0;
extern int D_8009B1FC[], D_8009B0FC[];
extern char *D_8009AFDC[], *D_8009B05C[];
/* BIOS_1 exports these as CD_pos, CD_mode and CD_com. */
extern u8 D_8009AFD0[4], D_8009AFD4, D_8009AFD5;
extern volatile u8 *D_8009B27C, *D_8009B280, *D_8009B284;
extern u8 D_800A3460[8], D_800A3468[8];
extern int D_800A3478, D_800A347C;
extern char *D_800A3480;
extern char D_80011BB4[], D_80011BBC[], D_80011BCC[];
extern char D_80011B18[], D_80011B28[];
int printf(const char *format, ...);
int puts(const char *text);
void CD_flush(void);
int getintr(void);

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

int CD_cw(int command, void *parameters, u8 *result, int mode) {
    u8 *parameter = parameters;
    int i;
    if (D_8009AFC0 >= 2)
        printf(D_80011BB4, D_8009AFDC[(u8)command]);
    if (D_8009B1FC[(u8)command] && !parameter) {
        if (D_8009AFC0 > 0)
            printf(D_80011BBC, D_8009AFDC[(u8)command]);
        return -2;
    }
    CD_sync(0, 0);
    if ((u8)command == 2) {
        for (i = 0; i < 4; i++) D_8009AFD0[i] = parameter[i];
    }
    if ((u8)command == 14) D_8009AFD4 = parameter[0];
    D_8009B294.sync = 0;
    if (D_8009B0FC[(u8)command]) D_8009B294.ready = 0;
    *D_8009B27C = 0;
    for (i = 0; i < D_8009B1FC[(u8)command]; i++)
        *D_8009B284 = parameter[i];
    D_8009AFD5 = command;
    *D_8009B280 = command;
    if (mode) return 0;

    D_800A3478 = VSync(-1) + 0x3C0;
    D_800A347C = 0;
    D_800A3480 = D_80011BCC;
    if (!D_8009B294.sync) {
        char **commands = D_8009AFDC;
        char **events = D_8009B05C;

        do {
            if (timed_out(commands, events)) return -1;
            if (CheckCallback()) {
                int bank = *D_8009B27C & 3;
                int pending;
                while ((pending = getintr()) != 0) {
                    if ((pending & 4) && D_8009AFB8)
                        D_8009AFB8(D_8009B294.ready, D_800A3468);
                    if ((pending & 2) && D_8009AFB4)
                        D_8009AFB4(D_8009B294.sync, D_800A3460);
                }
                *D_8009B27C = bank;
            }
        } while (!D_8009B294.sync);
    }
    copy_result(result, D_800A3460);
    {
        int status = 0;
        if (D_8009B294.sync == 5) status = -1;
        return status;
    }
}
