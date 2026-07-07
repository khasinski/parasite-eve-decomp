/* CC1_FLAGS: -g3 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void Entity_WriteFieldByCmd(void **arg0, s32 arg1, s32 arg2) {
    s32 temp_a1;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v1;
    register s32 var_v1_2 asm("$3");
    void *temp_a0;

    temp_a0 = *arg0;
    temp_a1 = arg1 & 0xFF;
    switch (temp_a1) {                              /* switch 1 */
    case 0x28:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s16 *, 0xE) = arg2;
        return;
    case 0x29:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 4) = arg2;
        return;
    case 0x2A:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 5) = arg2;
        return;
    case 0x2C:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s32 *, 0x10) = arg2;
        return;
    case 0x2D:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s32 *, 0x14) = arg2;
        return;
    case 0x2B:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s16 *, 0xC) = arg2;
        return;
    case 0x2E:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 7) = arg2;
        return;
    case 0x2F:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 6) = arg2;
        return;
    case 0x3C:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s32 *, 0x88) = arg2;
        return;
    case 0x3D:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s16 *, 0x8C) = arg2;
        return;
    case 0x3E:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s16 *, 0x8E) = arg2;
        return;
    case 0x3F:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 0x90) = arg2;
        return;
    case 0x40:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 0x91) = arg2;
        return;
    case 0x41:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 0x92) = arg2;
        return;
    case 0x42:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 0x93) = arg2;
        return;
    case 0x43:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 0x94) = arg2;
        return;
    case 0x44:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 0x95) = arg2;
        return;
    case 0x45:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s16 *, 0x96) = arg2;
        return;
    case 0x4A:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s32 *, 0) = (s32) ((M2C_FIELD(temp_a0, s32 *, 0) & ~0x10) | ((arg2 & 1) * 0x10));
        return;
    case 0x50:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 0xA4) = arg2;
        switch (arg2) {                             /* switch 2; irregular */
        case 1:                                     /* switch 2 */
            M2C_FIELD(temp_a0, s16 *, 0xA6) = 0x190;
            return;
        case 2:                                     /* switch 2 */
            M2C_FIELD(temp_a0, s16 *, 0xA6) = 0x46;
            return;
        case 3:                                     /* switch 2 */
            M2C_FIELD(temp_a0, s16 *, 0xA6) = 0x14;
            return;
        }
        break;
    case 0x51:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s32 *, 0) = (s32) ((M2C_FIELD(temp_a0, s32 *, 0) & 0xFFEFFFFF) | ((arg2 & 1) << 0x14));
        return;
    case 0x5A:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s16 *, 0x98) = arg2;
        return;
    case 0x5B:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s16 *, 0x9A) = arg2;
        return;
    case 0x5C:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 0x9E) = arg2;
        return;
    case 0x5D:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 0x9F) = arg2;
        return;
    case 0x5E:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 0xAE) = arg2;
        return;
    case 0x5F:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 0xAF) = arg2;
        return;
    case 0x60:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 0xBC) = arg2;
        return;
    case 0x32:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s16 *, 0xB0) = arg2;
        return;
    case 0x33:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s16 *, 0xB2) = arg2;
        return;
    case 0x34:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s16 *, 0xB4) = arg2;
        return;
    case 0x35:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s16 *, 0xB6) = arg2;
        return;
    case 0x36:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s16 *, 0xB8) = arg2;
        return;
    case 0x37:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s16 *, 0xBA) = arg2;
        return;
    case 0x64:                                      /* switch 1 */
        var_v1 = ~3;
        var_v0 = M2C_FIELD(temp_a0, s32 *, 0xCC) & var_v1;
        var_v1 = arg2 & 3;
        goto block_51;
    case 0x65:                                      /* switch 1 */
        var_v1 = ~0xC;
        var_v0 = M2C_FIELD(temp_a0, s32 *, 0xCC) & var_v1;
        var_v1 = arg2 & 3;
        var_v1 <<= 2;
        goto block_51;
    case 0x66:                                      /* switch 1 */
        var_v1 = ~0x30;
        var_v0 = M2C_FIELD(temp_a0, s32 *, 0xCC) & var_v1;
        var_v1 = arg2 & 3;
        var_v1 <<= 4;
        goto block_51;
    case 0x67:                                      /* switch 1 */
        var_v1 = ~0xC0;
        var_v0 = M2C_FIELD(temp_a0, s32 *, 0xCC) & var_v1;
        var_v1 = arg2 & 3;
        var_v1 <<= 6;
        goto block_51;
    case 0x68:                                      /* switch 1 */
        var_v1 = ~0x300;
        var_v0 = M2C_FIELD(temp_a0, s32 *, 0xCC) & var_v1;
        var_v1 = arg2 & 3;
        var_v1 <<= 8;
        goto block_51;
    case 0x69:                                      /* switch 1 */
        var_v1 = ~0xC00;
        var_v0 = M2C_FIELD(temp_a0, s32 *, 0xCC) & var_v1;
        var_v1 = arg2 & 3;
        var_v1 <<= 0xA;
        goto block_51;
    case 0x6A:                                      /* switch 1 */
        var_v1 = ~0x3000;
        var_v0 = M2C_FIELD(temp_a0, s32 *, 0xCC) & var_v1;
        var_v1 = arg2 & 3;
        var_v1 <<= 0xC;
block_51:
        var_v0 |= var_v1;
        M2C_FIELD(temp_a0, s32 *, 0xCC) = var_v0;
        return;
    case 0x6B:                                      /* switch 1 */
        var_v0_2 = 0xFFFF3FFF;
        var_v1_2 = M2C_FIELD(temp_a0, s32 *, 0xCC) & var_v0_2;
        var_v0_2 = arg2 & 3;
        var_v0_2 <<= 0xE;
        goto block_56;
    case 0x6C:                                      /* switch 1 */
        var_v0_2 = 0xFFFCFFFF;
        var_v1_2 = M2C_FIELD(temp_a0, s32 *, 0xCC) & var_v0_2;
        var_v0_2 = arg2 & 3;
        var_v0_2 <<= 0x10;
        goto block_56;
    case 0x6D:                                      /* switch 1 */
        var_v0_2 = 0xFFFBFFFF;
        var_v1_2 = M2C_FIELD(temp_a0, s32 *, 0xCC) & var_v0_2;
        var_v0_2 = arg2 & 1;
        var_v0_2 <<= 0x12;
        goto block_56;
    case 0x6E:                                      /* switch 1 */
        var_v0_2 = 0xFF07FFFF;
        var_v1_2 = M2C_FIELD(temp_a0, s32 *, 0xCC) & var_v0_2;
        var_v0_2 = arg2 & 0x1F;
        var_v0_2 <<= 0x13;
block_56:
        var_v1_2 |= var_v0_2;
        M2C_FIELD(temp_a0, s32 *, 0xCC) = var_v1_2;
        return;
    case 0x78:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s16 *, 0xD0) = arg2;
        return;
    case 0x79:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 0xD6) = arg2;
        return;
    case 0x7A:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s8 *, 0xD7) = arg2;
        return;
    case 0x7B:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s16 *, 0xA0) = arg2;
        return;
    case 0x7C:                                      /* switch 1 */
        M2C_FIELD(temp_a0, s16 *, 0xA2) = arg2;
        /* fallthrough */
    default:                                        /* switch 1 */
        return;
    }
}
