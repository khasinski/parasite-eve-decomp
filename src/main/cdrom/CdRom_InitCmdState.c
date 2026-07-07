/* MASPSX_FLAGS: --stack-return-delay */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK CdIntToPos();
extern u8 g_DsReadSysEnabled[];
extern u8 D_8009B560[];
extern u8 g_CdSeekState[];

void CdRom_InitCmdState(void) {
    s32 var_v1;
    s32 var_v1_2;
    u8 *base554;
    register s8 *var_a0 asm("$4");
    s8 *var_v0;
    u8 *state;
    s32 value;
    s32 zeroArg;
    u8 *pos;

    var_v1 = 3;
    base554 = g_DsReadSysEnabled;
    var_a0 = base554 + 8;
    __asm__ volatile("" : "=r"(base554) : "0"(base554));
    M2C_FIELD(base554, s32 *, 0) = 0;
    M2C_FIELD(base554, s8 *, 4) = 0;
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
    state = g_CdSeekState;
    __asm__ volatile("" : "=r"(state) : "0"(state));
    value = 2;
    M2C_FIELD(state, s32 *, 8) = value;
    value = 0xE;
    M2C_FIELD(state, s32 *, 0xC) = value;
    value = 0x15;
    zeroArg = 0;
    pos = state + 0x16;
    M2C_FIELD(state, s8 *, 0) = 0;
    M2C_FIELD(state, s32 *, 4) = 0;
    M2C_FIELD(state, s32 *, 0x10) = value;
    M2C_FIELD(state, s8 *, 0x14) = 0;
    M2C_FIELD(state, s8 *, 0x15) = 0;
    CdIntToPos(zeroArg, pos);
    M2C_FIELD(state, s8 *, 0x1A) = 0;
    M2C_FIELD(state, s8 *, 0x1B) = 0;
    M2C_FIELD(state, s8 *, 0x1C) = 0;
    M2C_FIELD(state, s8 *, 0x1D) = 0;
    M2C_FIELD(state, s8 *, 0x1E) = 0;
    M2C_FIELD(state, s8 *, 0x1F) = 0;
    M2C_FIELD(state, s32 *, 0x20) = 0;
    M2C_FIELD(state, s32 *, 0x24) = 1;
    M2C_FIELD(state, s32 *, 0x28) = 0;
    M2C_FIELD(state, s32 *, 0x30) = 0;
    M2C_FIELD(state, s32 *, 0x2C) = 0;
    M2C_FIELD(state, s32 *, 0x34) = 0;
}
