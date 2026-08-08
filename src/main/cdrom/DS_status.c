/* MASPSX_FLAGS: --stack-return-delay --la-call-delay */

#include "pe1/psyq_cd.h"

typedef unsigned int u32;

extern char D_80011D74[];
extern char D_80011D94[];
extern char D_80011DB0[];
extern char D_80011DD8[];
extern char D_80011DF8[];
extern char D_80011E0C[];
extern char D_80011E3C[];
extern char D_80011E44[];
extern char D_80011E4C[];
extern DsReadStatusBlock g_DsReadStatusBlock asm("D_8009B574");
extern u32 g_DsPollCallback asm("D_800A36A0");
extern u32 g_DsSyncCallback asm("D_800A36A4");
extern u32 g_DsReadyCallback asm("D_800A36A8");

int DsRead_IsBusy(void);
int printf(char *fmt, ...);

void DS_status(void) {
    DsReadStatusBlock *state;
    int b0;
    int b1;
    int b2;
    int b3;
    int b4;
    char *busy;

    printf(D_80011D74);

    state = &g_DsReadStatusBlock;
    asm volatile("" : "=r"(state) : "0"(state));
    printf(D_80011D94, state->status, state->command, state->sector);
    b0 = ((u_char *)state)[-0x1C];
    b1 = ((u_char *)state)[-0x1B];
    b2 = ((u_char *)state)[-0x1A];
    b3 = ((u_char *)state)[-0x19];
    b4 = ((u_char *)state)[-0x18];
    printf(D_80011DB0, b0, b1, b2, b3, b4);
    printf(D_80011DD8, state->readyResult);
    printf(D_80011DF8, state->syncResult);
    printf(D_80011E0C, g_DsPollCallback, g_DsSyncCallback, g_DsReadyCallback);

    if (DsRead_IsBusy() != 0) {
        busy = D_80011E3C;
    } else {
        busy = D_80011E44;
    }
    printf(D_80011E4C, busy);
}
