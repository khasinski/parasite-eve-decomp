/* GCC_VERSION: 2.8.1 */
/* ASSEMBLER: GNU */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_cd.h"

extern char D_80011D74[];
extern char D_80011D94[];
extern char D_80011DB0[];
extern char D_80011DD8[];
extern char D_80011DF8[];
extern char D_80011E0C[];
extern char D_80011E3C[];
extern char D_80011E44[];
extern char D_80011E4C[];
extern DsReadStatusBlock g_DsReadStatusBlock __asm__("D_8009B574");
extern u32 D_800A36A0;
extern u32 D_800A36A4;
extern u32 D_800A36A8;

int DsRead_IsBusy(void);
int printf(char *fmt, ...);

void DS_status(void) {
    enum {
        PARAM_OFFSET = PE1_OFFSETOF(CdRomEventCommandState, pendingParamBytes) -
                       PE1_OFFSETOF(CdRomEventCommandState, command.read)
    };
    DsReadStatusBlock *state;
    CdRomEventCommandState *command;
    register int b0 asm("$5");
    register int b1 asm("$6");
    register int b2 asm("$7");
    register int b3 asm("$2");
    register int b4 asm("$3");
    register char *busy asm("$5");

    printf(D_80011D74);

    state = &g_DsReadStatusBlock;
    asm volatile("" : "=r"(state) : "0"(state));
    printf(D_80011D94, state->status, state->command, state->sector);
    command = (CdRomEventCommandState *)((char *)state -
        PE1_OFFSETOF(CdRomEventCommandState, command.read));
    b0 = command->pendingCommand;
    b1 = ((u_char *)state)[PARAM_OFFSET + 0];
    b2 = ((u_char *)state)[PARAM_OFFSET + 1];
    b3 = ((u_char *)state)[PARAM_OFFSET + 2];
    b4 = ((u_char *)state)[PARAM_OFFSET + 3];
    /* Preserve the five diagnostic argument loads in their ABI registers. */
    asm volatile("" : : "r"(b0), "r"(b1), "r"(b2), "r"(b3), "r"(b4));
    printf(D_80011DB0, b0, b1, b2, b3, b4);
    printf(D_80011DD8, state->readyResult);
    printf(D_80011DF8, state->syncResult);
    printf(D_80011E0C, D_800A36A0, D_800A36A4, D_800A36A8);

    {
        int active = DsRead_IsBusy();
        busy = D_80011E44;
        asm volatile("" : "=r"(busy) : "0"(busy));
        if (active != 0) busy = D_80011E3C;
    }
    printf(D_80011E4C, busy);
}
