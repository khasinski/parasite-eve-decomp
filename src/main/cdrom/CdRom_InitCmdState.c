#include "common.h"
#include "pe1/psyq_cd.h"
/* MASPSX_FLAGS: --stack-return-delay */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK CdIntToPos();
extern u8 D_8009B560[];

void CdRom_InitCmdState(void) {
    s32 var_v1;
    s32 var_v1_2;
    u8 *base554;
    register s8 *var_a0 asm("$4");
    s8 *var_v0;
    CdRomCommandState *state;
    s32 value;
    s32 zeroArg;
    u8 *pos;

    var_v1 = 3;
    base554 = (u8 *)&g_DsReadSysEnabled;
    var_a0 = base554 + 8;
    __asm__ volatile("" : "=r"(base554) : "0"(base554));
    ((CdRomSystemState *)base554)->enabled = 0;
    ((CdRomSystemState *)base554)->pendingCommand = 0;
    do {
        *var_a0 = 0;
        var_v1 -= 1;
        var_a0 -= 1;
    } while (var_v1 >= 0);
    M2C_FIELD(D_8009B560, s32 *, 0) = 0;
    var_v1_2 = 7;
    var_v0 = D_8009B560 + 0xB;
    do {
        *var_v0 = 0;
        var_v1_2 -= 1;
        var_v0 -= 1;
    } while (var_v1_2 >= 0);
    state = &g_CdSeekState;
    __asm__ volatile("" : "=r"(state) : "0"(state));
    value = 2;
    state->read.status = value;
    value = 0xE;
    state->read.command = value;
    value = 0x15;
    zeroArg = 0;
    pos = (u8 *)&state->read.currentPos;
    state->eventStatus = 0;
    state->eventValue = 0;
    state->read.sector = value;
    state->read.lastCommand = 0;
    state->read.commandMode = 0;
    CdIntToPos(zeroArg, pos);
    state->read.retryCount = 0;
    state->read.commandParam = 0;
    state->read.eventFlags.bit7 = 0;
    state->read.eventFlags.bit6 = 0;
    state->read.eventFlags.bit5 = 0;
    state->read.eventFlags.bit1 = 0;
    state->read.reserved18 = 0;
    state->read.discType = 1;
    state->read.syncResult = 0;
    state->retryAttempts = 0;
    state->read.readyResult = 0;
    state->reserved34 = 0;
}
