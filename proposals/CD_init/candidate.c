/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_cd.h"

extern char D_80011C08[], D_80011C14[], D_8009B298[];
extern u8 D_8009AFD5, D_8009AFD4;
extern volatile u8 *D_8009B27C, *D_8009B288, *D_8009B284;
/* Same word-control address/type as g_CdRegRequest; one initialization store. */
extern u32 *D_8009B28C;
extern void Cd_SetIntrMask(void);
int puts(const char *text);
int printf(const char *format, ...);

int CD_init(void) {
    puts(D_80011C08);
    printf(D_80011C14, D_8009B298);
    D_8009AFD5 = 0;
    D_8009AFD4 = 0;
    D_8009AFB8 = 0;
    D_8009AFB4 = 0;
    D_8009AFC8 = 0;
    D_8009AFC4 = 0;
    ResetCallback();
    InterruptCallback(2, Cd_SetIntrMask);
    *D_8009B27C = 1;
    while (*D_8009B288 & 7) {
        *D_8009B27C = 1;
        *D_8009B288 = 7;
        *D_8009B284 = 7;
    }
    {
        CdInterruptEvents *state = &D_8009B294;
        volatile u8 *index;

        state->end = 0;
        state->ready = state->end;
        index = D_8009B27C;
        state->sync = 2;
        *index = 0;
    }
    *D_8009B288 = 0;
    *D_8009B28C = 0x1325;
    CD_cw(1, 0, 0, 0);
    if (D_8009AFC4 & 0x10) CD_cw(1, 0, 0, 0);
    if (CD_cw(10, 0, 0, 0)) return -1;
    if (CD_cw(12, 0, 0, 0)) return -1;
    {
        int status = CD_sync(0, 0);
        if (status != 2) return -1;
        return 0;
    }
}
