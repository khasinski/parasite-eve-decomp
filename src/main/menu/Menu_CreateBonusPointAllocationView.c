/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Stat_QueryLevelAndSubLevel();
void *func_80062D2C();
extern struct { char _[16]; } D_80092258_o __asm__("D_80092258");
#define D_80092258 (*(M2C_UNK *)&D_80092258_o)
extern struct { char _[16]; } D_80092298_o __asm__("D_80092298");
#define D_80092298 (*(M2C_UNK *)&D_80092298_o)
extern s32 g_BonusPointDisplayValue;
extern struct { char _[16]; } D_800A18B4_o __asm__("g_BonusPointStatQueryResults");
#define g_BonusPointStatQueryResults (*(M2C_UNK *)&D_800A18B4_o)
extern struct { char _[16]; } D_800A18D8_o __asm__("g_BonusPointStatDeltas");
#define g_BonusPointStatDeltas (*(s32 *)&D_800A18D8_o)
extern struct { char _[16]; } D_800A18FC_o __asm__("g_BonusPointStatMultipliers");
#define g_BonusPointStatMultipliers (*(s32 *)&D_800A18FC_o)
extern struct { char _[16]; } D_800C0E28_o __asm__("g_AyaStatAgility");
#define g_AyaStatAgility (*(u16 *)&D_800C0E28_o)
extern struct { char _[16]; } func_80043B0C_o __asm__("Menu_DrawStatusPanel");
#define Menu_DrawStatusPanel (*(M2C_UNK *)&func_80043B0C_o)
extern struct { char _[16]; } func_80043C64_o __asm__("Menu_DrawStatsList");
#define Menu_DrawStatsList (*(M2C_UNK *)&func_80043C64_o)
extern struct { char _[16]; } func_8004905C_o __asm__("Menu_DrawBonusPointSlotValue");
#define Menu_DrawBonusPointSlotValue (*(M2C_UNK *)&func_8004905C_o)
extern struct { char _[16]; } g_AyaBonusPoints_o __asm__("g_AyaBonusPoints");
#define g_AyaBonusPoints (*(s32 *)&g_AyaBonusPoints_o)

asm(".globl Menu_CreateBonusPointAllocationView");
asm("Menu_CreateBonusPointAllocationView = func_800439D8");

void Menu_CreateBonusPointAllocationView(void) __asm__("func_800439D8");

void Menu_CreateBonusPointAllocationView(void) {
    register M2C_UNK *temp_a2 asm("$6");
    u8 *var_s3;
    u8 *var_s0;
    u8 *var_s2;
    register s32 temp_a0 asm("$4");
    s32 temp_a3;
    s32 temp_a1;
    s32 var_s1;
    u8 *var_s4;
    u16 temp_v0_3;
    s32 temp_v0_final;
    void *temp_v0;
    void *temp_v0_2;
    register void *temp_v1_reg asm("$3");

    temp_v0 = func_80062D2C(0x12, 0, 0, 0);
    temp_v1_reg = temp_v0;
    M2C_FIELD(temp_v1_reg, M2C_UNK **, 0x30) = &Menu_DrawStatusPanel;
    M2C_FIELD(temp_v1_reg, M2C_UNK **, 0x4C) = &D_80092258;
    M2C_FIELD(func_80062D2C(0x18, 0, 0, 0), M2C_UNK **, 0x30) = &Menu_DrawBonusPointSlotValue;
    temp_v0_2 = func_80062D2C(0x2D, 0, 0, 0);
    temp_v1_reg = temp_v0_2;
    var_s4 = (u8 *) &g_AyaStatAgility;
    var_s1 = 0;
    var_s0 = (u8 *) &g_BonusPointStatDeltas;
    var_s3 = (u8 *) &g_BonusPointStatQueryResults;
    var_s2 = (u8 *) &g_BonusPointStatMultipliers;
    M2C_FIELD(temp_v1_reg, M2C_UNK **, 0x30) = &Menu_DrawStatsList;
    M2C_FIELD(temp_v1_reg, M2C_UNK **, 0x4C) = &D_80092298;
    do {
        temp_v0_3 = M2C_FIELD(var_s4, u16 *, 0);
        var_s4 += 2;
        temp_a0 = var_s1;
        temp_a2 = var_s3;
        temp_a3 = 0;
        __asm__ volatile("" : "=r"(temp_a3) : "0"(temp_a3));
        var_s3 += 4;
        var_s1 += 1;
        M2C_FIELD(var_s0, s32 *, 0) = (s32) temp_v0_3;
        M2C_FIELD(var_s2, s32 *, 0) = 0;
        temp_a1 = M2C_FIELD(var_s0, s32 *, 0);
        var_s0 += 4;
        var_s2 += 4;
        Stat_QueryLevelAndSubLevel(temp_a0, temp_a1, temp_a2, temp_a3);
    } while (var_s1 < 7);
    temp_v0_final = g_AyaBonusPoints;
    __asm__ volatile("nop");
    g_BonusPointDisplayValue = temp_v0_final;
}
