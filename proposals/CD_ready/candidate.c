/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-expensive-optimizations */
#include "../libcd_bios_helpers.h"

extern char *D_8009AFDC[], *D_8009B05C[];
extern volatile u8 *D_8009B27C;
extern u8 D_800A3460[8], D_800A3468[8], D_800A3470[8];
extern char D_80011BA8[];
int getintr(void);

int CD_ready(int mode, u8 *result) {
    char **commands;
    char **events;
    D_800A3478 = VSync(-1) + 0x3C0;
    commands = D_8009AFDC;
    events = D_8009B05C;
    D_800A347C = 0;
    D_800A3480 = D_80011BA8;
    do {
        int status;
        if (timed_out(commands, events)) return -1;
        if (CheckCallback()) {
            dispatch_interrupts();
        }
        status = D_8009B294.end;
        if (status) {
            D_8009B294.end = 0;
            copy_result(result, D_800A3470);
            return status;
        }
        status = D_8009B294.ready;
        if (status) {
            D_8009B294.ready = 0;
            copy_result(result, D_800A3468);
            return status;
        }
    } while (!mode);
    return 0;
}
