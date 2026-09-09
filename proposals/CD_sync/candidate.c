/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "../libcd_bios_helpers.h"

extern char *D_8009AFDC[], *D_8009B05C[];
extern volatile u8 *D_8009B27C;
extern u8 D_800A3460[8], D_800A3468[8];
extern char D_80011BA0[];
int getintr(void);

int CD_sync(int mode, u8 *result) {
    char **commands;
    char **events;
    D_800A3478 = VSync(-1) + 0x3C0;
    commands = D_8009AFDC;
    events = D_8009B05C;
    D_800A347C = 0;
    D_800A3480 = D_80011BA0;
    do {
        int status;
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
        status = D_8009B294.sync;
        if (status == 2 || status == 5) {
            D_8009B294.sync = 2;
            copy_result(result, D_800A3460);
            return status;
        }
    } while (!mode);
    return 0;
}
