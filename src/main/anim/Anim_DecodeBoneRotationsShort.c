typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void RotMatrixYXZ();

void Anim_DecodeBoneRotationsShort(void *arg0, void *arg1, s16 arg2) {
    register u16 *var_s2 asm("$18");
    s16 var_s3;
    s16 temp_v0;
    s16 temp_v0_2;
    register s32 nlim asm("$3");
    s32 var_v0;
    s32 *var_a0;
    s32 temp_a3;
    s32 temp_s5;
    s32 temp_s6;
    char *var_s4;
    u16 var_v0_2;
    u16 var_v0_3;
    u16 var_v0_4;
    char *var_s0;
    char *var_s1;
    char *var_v1;

    var_s2 = (u16 *)0x1F800000;
    var_s0 = (char *)arg1 + 0xC;
    var_s3 = 0;
    var_s4 = (char *) M2C_FIELD(arg0, s32 *, 0x84);
    M2C_FIELD(arg0, s32 *, 0x58) = (s32) var_s4;
    var_a0 = (s32 *) (var_s4 + 0x14);
    temp_a3 = ((u8) M2C_FIELD(arg1, u8 *, 2) >> 1) + 1;
    do {
        var_v1 = var_s0 + 2;
        if (M2C_FIELD(var_s0, s16 *, 0) != 0) {
            var_v0 = M2C_FIELD(var_s0, s16 *, 2);
            var_s0 += 4;
        } else {
            var_v0 = *(s16 *)((((s32) (arg2 << 0x10) >> 0xF)) + (s32) var_v1);
            var_s0 += temp_a3 * 4;
        }
        *var_a0 = (s32) var_v0;
        temp_v0 = var_s3 + 1;
        var_s3 = temp_v0;
        var_a0 += 1;
    } while (temp_v0 < 3);
    var_s3 = 0;
    if ((s32) M2C_FIELD(arg1, u8 *, 1) >= var_s3) {
        temp_s6 = ((s32) (arg2 << 0x10) >> 0xF);
        temp_s5 = temp_a3 * 4;
        var_s1 = (char *)var_s2 + 4;
        do {
            var_v1 = var_s0 + 2;
            if (M2C_FIELD(var_s0, s16 *, 0) != 0) {
                var_v0_2 = M2C_FIELD(var_s0, u16 *, 2);
                var_s0 = var_s0 + 4;
            } else {
                var_v0_2 = *(u16 *)(temp_s6 + (s32) var_v1);
                var_s0 = var_s0 + temp_s5;
            }
            *var_s2 = var_v0_2;
            var_v1 = var_s0 + 2;
            if (M2C_FIELD(var_s0, s16 *, 0) != 0) {
                var_v0_3 = M2C_FIELD(var_s0, u16 *, 2);
                var_s0 = var_s0 + 4;
            } else {
                var_v0_3 = *(u16 *)(temp_s6 + (s32) var_v1);
                var_s0 = var_s0 + temp_s5;
            }
            M2C_FIELD(var_s1, u16 *, -2) = var_v0_3;
            var_v1 = var_s0 + 2;
            if (M2C_FIELD(var_s0, s16 *, 0) != 0) {
                var_v0_4 = M2C_FIELD(var_s0, u16 *, 2);
                var_s0 = var_s0 + 4;
            } else {
                var_v0_4 = *(u16 *)(temp_s6 + (s32) var_v1);
                var_s0 = var_s0 + temp_s5;
            }
            M2C_FIELD(var_s1, u16 *, 0) = var_v0_4;
            __asm__ volatile("");
            RotMatrixYXZ(var_s2, var_s4);
            var_s4 += 0x20;
            var_s1 += 8;
            temp_v0_2 = var_s3 + 1;
            var_s3 = temp_v0_2;
            var_s2 += 4;
            nlim = M2C_FIELD(arg1, u8 *, 1);
        } while (nlim >= var_s3);
    }
}
