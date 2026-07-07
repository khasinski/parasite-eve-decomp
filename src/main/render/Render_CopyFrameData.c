/* CC1_FLAGS: -g3 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

void Render_CopyFrameData(void *arg0, void *arg1, s32 arg2) {
    s32 temp_a2;

    temp_a2 = (s32) (arg2 << 0x10) >> 0xB;
    M2C_FIELD(arg0, u16 *, 0x34) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 0);
    M2C_FIELD(arg0, u16 *, 0x36) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 2);
    M2C_FIELD(arg0, u16 *, 0x38) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 4);
    M2C_FIELD(arg0, u16 *, 0x3A) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 6);
    M2C_FIELD(arg0, u16 *, 0x3C) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 8);
    M2C_FIELD(arg0, u16 *, 0x3E) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 0xA);
    M2C_FIELD(arg0, u16 *, 0x40) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 0xC);
    M2C_FIELD(arg0, u16 *, 0x42) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 0xE);
    M2C_FIELD(arg0, u16 *, 0x44) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 0x10);
    M2C_FIELD(arg0, s32 *, 0x48) = (s32) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), s32 *, 0x14);
    M2C_FIELD(arg0, s32 *, 0x4C) = (s32) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), s32 *, 0x18);
    M2C_FIELD(arg0, s32 *, 0x50) = (s32) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), s32 *, 0x1C);
}
