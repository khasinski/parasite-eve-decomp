/* ASSEMBLER: GNU */
/* CC1_FLAGS: -fno-expensive-optimizations */
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
    register int zero asm("$0");
    int result;
    register int cmd asm("$4");
    register int arg asm("$5");

    puts(D_80011C08);
    asm volatile("" : "=r"(zero));
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
        volatile u8 *index;
        CdInterruptEvents *state;
        cmd = 1;
        arg = zero;
        /* Debt: prepare a0/a1 before the event-state address. */
        asm volatile("" : : "r"(cmd), "r"(arg));
        state = &D_8009B294;
        /* Debt: retain one shared event-state base. */
        asm volatile("" : "=r"(state) : "0"(state));

        state->end = 0;
        state->ready = state->end;
        index = D_8009B27C;
        state->sync = 2;
        *index = 0;
    }
    *D_8009B288 = 0;
    *D_8009B28C = 0x1325;
    CD_cw(cmd, arg, zero, zero);
    if (D_8009AFC4 & 0x10) CD_cw(1, zero, zero, zero);
    if (CD_cw(10, zero, zero, zero)) return -1;
    if (CD_cw(12, zero, zero, zero)) goto fail;
    {
        register int status asm("$4") = CD_sync(zero, zero);
        register int two asm("$3") = 2;
        asm volatile("" : "=r"(status), "=r"(two) : "0"(status), "1"(two));
        if (status != two) return -1;
        asm volatile("");
        result = 0;
        goto done;
    }
fail:
    asm volatile("");
    result = -1;
done:
    return result;
}
