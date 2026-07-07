typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

s32 Battle_GetEnemyContextField(void *arg0, s32 arg1) {
    u8 *ctx = *(u8 **)arg0;
    s32 ret = -1000;
    register u32 word asm("$2");
    s32 flags;

    switch (((u8)arg1) - 0x29) {
    case 0:
        ret = *(s8 *)(ctx + 4);
        break;
    case 2:
        ret = *(u16 *)(ctx + 0xC);
        break;
    case 3:
        ret = *(s32 *)(ctx + 0x10);
        if (ret < 0) {
            ret = 0;
        }
        break;
    case 7:
        word = *(u32 *)(ctx + 0);
        word >>= 17;
        word &= 0x70;
        word = (u32)ctx + word;
        ret = *(u8 *)(word + 0x1C);
        break;
    case 19:
        ret = *(s32 *)(ctx + 0x88);
        if (ret < 0) {
            ret = 0;
        }
        break;
    case 20:
        ret = *(u16 *)(ctx + 0x8C);
        break;
    case 36:
        word = *(u32 *)(ctx + 0);
        ret = word >> 13;
        ret &= 3;
        break;
    case 41:
        ret = *(u8 *)(ctx + 3);
        ret &= 0x3F;
        break;
    case 89:
        flags = *(s32 *)(ctx + 0xCC);
        if (flags & 0x1000000) {
            word = *(s32 *)(ctx + 0) & 0xC0000;
            if (word == 0xC0000) {
                *(s32 *)(ctx + 0xCC) = flags & 0xFEFFFFFF;
                ret = 1;
            } else {
                ret = 0;
            }
        } else {
            ret = 0;
        }
        break;
    }
    return ret;
}
