/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_cd.h"

extern volatile u8 *D_8009B27C;
extern volatile u8 cd_ready_event __asm__("D_8009B295");
extern u8 D_800A3460[];
extern u8 D_800A3468[];

int getintr(void);

void Cd_SetIntrMask(void) {
    int pending;
    volatile u8 *ready = &cd_ready_event;
    int bank = *D_8009B27C & 3;
    volatile u8 *complete = ready - 1;

    while ((pending = getintr()) != 0) {
        if ((pending & 4) && D_8009AFB8) {
            D_8009AFB8(*ready, D_800A3468);
        }
        if ((pending & 2) && D_8009AFB4) {
            D_8009AFB4(*complete, D_800A3460);
        }
    }

    *D_8009B27C = bank;
}
