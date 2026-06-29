/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

void Render_CopyFrameDataDouble(void *arg0, void *arg1, s32 arg2) {
    s32 temp_a2;

    temp_a2 = (s32) (arg2 << 0x10) >> 0xB;
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 0) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 0);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 2) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 2);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 4) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 4);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 6) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 6);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 8) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 8);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 0xA) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 0xA);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 0xC) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 0xC);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 0xE) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 0xE);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 0x10) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 0x10);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), s32 *, 0x14) = (s32) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), s32 *, 0x14);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), s32 *, 0x18) = (s32) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), s32 *, 0x18);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), s32 *, 0x1C) = (s32) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), s32 *, 0x1C);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 0x20) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 0);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 0x22) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 2);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 0x24) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 4);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 0x26) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 6);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 0x28) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 8);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 0x2A) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 0xA);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 0x2C) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 0xC);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 0x2E) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 0xE);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), u16 *, 0x30) = (u16) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), u16 *, 0x10);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), s32 *, 0x34) = (s32) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), s32 *, 0x14);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), s32 *, 0x38) = (s32) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), s32 *, 0x18);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x84), s32 *, 0x3C) = (s32) M2C_FIELD((temp_a2 + M2C_FIELD(arg1, s32 *, 0x84)), s32 *, 0x1C);
}
