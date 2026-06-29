/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

s32 func_8006FC18();

extern struct { char _[16]; } D_800942E4_o __asm__("g_PmSlotTable");
#define g_PmSlotTable (*(s32 *)&D_800942E4_o)
extern struct { char _[16]; } D_800942E8_o __asm__("g_PmSlotTable2");
#define g_PmSlotTable2 (*(s32 *)&D_800942E8_o)
extern struct { char _[16]; } g_PlayerEntity_o __asm__("g_PlayerEntity");
#define g_PlayerEntity (*(s32 *)&g_PlayerEntity_o)
extern s32 D_800B0CD8_r[] __asm__("g_GameState");
extern s32 D_800B0CD8_w[] __asm__("g_GameState");
extern struct { char _[16]; } D_800E0EF0_o __asm__("g_PmSlotBuffer");
#define g_PmSlotBuffer (*(M2C_UNK *)&D_800E0EF0_o)

s32 Pm_StopAll(void) {
    u8 *clear_base;
    u8 *base_v0;
    s32 *var_v1;
    s32 var_a2;
    s32 var_s1;
    s32 var_s2;
    s32 var_v0;
    u32 var_a0;
    u32 var_s0;
    u8 temp_v1;
    void *var_a1;

    var_a2 = 0;
    var_s0 = 0;
    clear_base = (u8 *)&g_PmSlotBuffer;
    var_s2 = -0xB84;
    var_s1 = 0;
loop_1:
    asm volatile(
        "lui %0,%%hi(g_PmSlotTable)\n"
        "lw %0,%%lo(g_PmSlotTable)(%0)"
        : "=r"(base_v0));
    temp_v1 = M2C_FIELD((base_v0 + var_s1), u8 *, 1);
    if ((temp_v1 < 8U) || ((u32)(temp_v1 - 0x55) < 0x1EU)) {
        var_v0 = func_8006FC18(var_s0, g_PlayerEntity, 1);
        var_a2 = var_v0;
        asm volatile("" : "=r"(var_a2) : "0"(var_a2));
        if (var_a2 == 0) {
            if (var_s0 < 0x16U) {
                if (var_s0 >= 0xBU) {
                    var_a1 = g_PmSlotTable2 + var_s2;
                } else {
                    asm volatile(
                        "lui %0,%%hi(g_PmSlotTable)\n"
                        "lw %0,%%lo(g_PmSlotTable)(%0)"
                        : "=r"(base_v0));
                    var_a1 = base_v0 + var_s1;
                }
                if (M2C_FIELD(var_a1, u8 *, 1) == 0x72) {
                    var_a0 = 0x6C;
                    var_v1 = (s32 *)(clear_base + 0x1B0);
                    do {
                        *var_v1 = 0;
                        var_a0 += 1;
                        var_v1 += 1;
                    } while (var_a0 < 0x73U);
                    D_800B0CD8_w[0] = D_800B0CD8_r[0] & 0xFFFEFFFF;
                }
                M2C_FIELD(var_a1, s8 *, 0) = 0;
                M2C_FIELD(var_a1, u8 *, 1) = 0xFFU;
                M2C_FIELD(var_a1, u8 *, 2) = 0xFFU;
                M2C_FIELD(var_a1, u8 *, 3) = 0xFFU;
                M2C_FIELD(var_a1, s32 *, 4) = 0;
                M2C_FIELD(var_a1, s32 *, 8) = 0;
            }
            goto block_13;
        }
    } else {
block_13:
        var_s2 += 0x10C;
        var_s0 += 1;
        var_s1 += 0xA0C;
        if ((s32)var_s0 >= 0xB) {
            var_v0 = var_a2;
        } else {
            goto loop_1;
        }
    }
    return var_v0;
}
