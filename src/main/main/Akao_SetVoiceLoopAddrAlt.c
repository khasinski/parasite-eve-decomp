/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK SeqOp_SetVoiceInstrument();
extern struct { char _[16]; } g_AkaoCurTrack_o __asm__("g_AkaoCurTrack");
#define g_AkaoCurTrack (*(s32 **)&g_AkaoCurTrack_o)
extern struct { char _[16]; } D_800B2900_o __asm__("g_AkaoInstrumentTable");
extern struct { char _[16]; } D_800B2900_o2 __asm__("g_AkaoInstrumentTable");

void Akao_SetVoiceLoopAddrAlt(void *arg0, u32 arg1) {
    s32 temp_a1_2;
    s32 temp_a2;
    s32 var_a0;
    s32 var_v1;
    s32 temp_a1;
    s32 temp_a0_2;
    u16 var_a1;
    u8 temp_a0;
    u8 temp_v0;
    s32 temp_v1;
    u8 tmp6;
    void *var_s0;

    var_a0 = 1;
    var_s0 = M2C_FIELD(arg0, void **, 0x18);
    temp_a2 = -((*g_AkaoCurTrack & 0x100) != 0) & 0x30;
    while ((u8) M2C_FIELD(var_s0, u8 *, 0) < 0x80U) {
        var_s0 += 8;
        var_a0 += 1;
    }
    var_s0 = M2C_FIELD(arg0, void **, 0x18);
    var_v1 = var_a0;
    __asm__("" : "=r"(var_v1) : "0"(var_v1));
    var_s0 = var_s0 + ((var_v1 - 1) * 8);
    if (var_v1 != 0) {
loop_4:
        if (arg1 < (u8) M2C_FIELD(var_s0, u8 *, 1)) {
            var_v1 -= 1;
            var_s0 -= 8;
            if (var_v1 != 0) {
                goto loop_4;
            }
        }
    }
    temp_a0 = M2C_FIELD(var_s0, u8 *, 0);
    temp_a1 = M2C_FIELD(arg0, u16 *, 0x5A);
    if (temp_a0 < 0x20U) {
        if (temp_a1 != temp_a0) {
            goto block_10;
        }
    } else if (temp_a1 != (temp_a0 + temp_a2)) {
block_10:
        if (var_v1 != 0) {
            temp_v1 = M2C_FIELD(var_s0, u8 *, -8);
            temp_a0_2 = M2C_FIELD(arg0, u16 *, 0x5A);
            if (temp_v1 < 0x20U) {
                if (temp_a0_2 != temp_v1) {
                    goto block_15;
                }
            } else if (temp_a0_2 != (temp_v1 + temp_a2)) {
                goto block_15;
            }
        } else {
block_15:
            temp_v0 = M2C_FIELD(var_s0, u8 *, 0);
            var_a1 = temp_v0 & 0xFF;
            if (temp_v0 >= 0x20U) {
                var_a1 += temp_a2;
            }
            M2C_FIELD(arg0, u16 *, 0x5A) = var_a1;
            temp_a1_2 = var_a1 << 6;
            SeqOp_SetVoiceInstrument(arg0, (u8 *)&D_800B2900_o + temp_a1_2, *(s32 *)((u8 *)&D_800B2900_o2 + temp_a1_2));
            M2C_FIELD(arg0, s16 *, 0x10E) = (s16) M2C_FIELD(var_s0, u8 *, 3);
            M2C_FIELD(arg0, s16 *, 0x114) = (s16) M2C_FIELD(var_s0, u8 *, 4);
            M2C_FIELD(arg0, s32 *, 0x104) = (s32) M2C_FIELD(var_s0, u8 *, 5);
            tmp6 = M2C_FIELD(var_s0, u8 *, 6);
            M2C_FIELD(arg0, s32 *, 0xF4) = (s32) (M2C_FIELD(arg0, s32 *, 0xF4) | 0x4000);
            M2C_FIELD(arg0, s16 *, 0x116) = tmp6;
        }
    }
}
