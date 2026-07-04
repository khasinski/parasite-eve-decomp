/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

M2C_UNK Akao_WriteVoiceParam();

extern M2C_UNK g_AkaoVoiceStateTable[];
#define g_AkaoVoiceStateTable (g_AkaoVoiceStateTable[0])

void Spu_CopyVoiceToStereoSlot(void *arg0, s32 arg1) {
    s32 temp_a0;
    s32 temp_a0_2;
    s32 temp_v0;
    register s32 temp_v1 asm("$3");
    void *temp_s0;

    temp_a0 = M2C_FIELD(arg0, s16 *, 0x118);
    temp_v1 = 0x7F;
    temp_v0 = M2C_FIELD(arg0, u16 *, 0xD8);
    temp_v0 = (s32)(temp_v0 << 0x10) >> 0x18;
    temp_v1 -= temp_v0;
    M2C_FIELD(arg0, s16 *, 0x118) = (s16)((u32)(temp_a0 * temp_v1) >> 8);
    temp_s0 = (arg1 * 0x11C) + (u8 *)&g_AkaoVoiceStateTable;
    M2C_FIELD(temp_s0, s16 *, 0x118) = (s16)((s32)(temp_a0 * (s16)M2C_FIELD(arg0, u16 *, 0xD8)) >> 0x10);
    temp_a0_2 = M2C_FIELD(arg0, s16 *, 0x11A);
    M2C_FIELD(arg0, s16 *, 0x11A) = (s16)((u32)(temp_a0_2 * temp_v1) >> 8);
    M2C_FIELD(temp_s0, s16 *, 0x11A) = (s16)((s32)(temp_a0_2 * (s16)M2C_FIELD(arg0, u16 *, 0xD8)) >> 0x10);
    M2C_FIELD(temp_s0, u16 *, 0x10C) = (u16)M2C_FIELD(arg0, u16 *, 0x10C);
    M2C_FIELD(temp_s0, s32 *, 0xF4) = (s32)(M2C_FIELD(temp_s0, s32 *, 0xF4) | M2C_FIELD(arg0, s32 *, 0xF4));
    Akao_WriteVoiceParam(M2C_FIELD(arg0, s32 *, 0xF0), arg0 + 0xF0, M2C_FIELD(arg0, s32 *, 0x38));
    Akao_WriteVoiceParam(arg1, temp_s0 + 0xF0, M2C_FIELD(arg0, s32 *, 0x38));
}
