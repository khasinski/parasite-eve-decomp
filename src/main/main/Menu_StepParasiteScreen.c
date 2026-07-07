/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 Aya_UnlockParasiteSpellById();
s32 MenuWidget_FindByModeAndSelectedBase();
M2C_UNK Menu_StepSaveLoadScreen();
M2C_UNK Inv_SetActiveList();
M2C_UNK Inv_RecalcSlotStats();
M2C_UNK Menu_PlayConfirmSound();
M2C_UNK Akao_FlushBgmVolumeFade();
s32 Menu_GetBattleCount();
M2C_UNK MenuWidget_DestroyNode();
M2C_UNK MenuWidget_NavScrollTo();
extern s32 g_BonusPointBarAnimProgress;
extern s32 g_MenuLevelDisplayValue;
extern s32 g_MenuHpMaxDisplayValue;
extern s32 g_BonusPointDisplayValue;
extern s32 g_MenuLevelDisplayTarget;
extern s32 g_MenuHpMaxDisplayTarget;
extern s32 g_MenuBonusPointDisplayTarget;
extern s32 g_MenuBonusPointBarAnimActive;
extern s32 g_BonusPointStatDeltas[];
#define g_BonusPointStatDeltas (g_BonusPointStatDeltas[0])
extern s32 g_BonusPointStatMultipliers[];
#define g_BonusPointStatMultipliers (g_BonusPointStatMultipliers[0])
extern s16 g_AyaHpMax[];
#define g_AyaHpMax (g_AyaHpMax[0])
extern s16 g_AyaStatAgility[];
#define g_AyaStatAgility (g_AyaStatAgility[0])
extern s32 g_AyaBonusPoints[];
#define g_AyaBonusPoints (g_AyaBonusPoints[0])
extern s8 g_AyaSaveLevel[];
#define g_AyaSaveLevel (g_AyaSaveLevel[0])

s32 Menu_StepParasiteScreen(s32 arg0, s32 arg1) {
    u8 *var_a3;
    u8 *var_a0_2;
    u8 *var_a1;
    s32 temp_v0;
    register s32 temp_v1 asm("$3");
    register s32 temp_bonus asm("$4");
    s32 var_a0;
    s32 var_a2_old;
    s32 var_a2;

    if (arg1 & 0x10000) {
        if (g_MenuBonusPointBarAnimActive != 0) {
            g_BonusPointBarAnimProgress = 0x80;
            g_MenuHpMaxDisplayValue = g_MenuHpMaxDisplayTarget;
            g_BonusPointDisplayValue = g_MenuBonusPointDisplayTarget;
            var_a2_old = g_MenuLevelDisplayValue;
            while (var_a2_old < g_MenuLevelDisplayTarget) {
                var_a0 = var_a2_old + 1;
                g_MenuLevelDisplayValue = var_a0;
                if (Aya_UnlockParasiteSpellById(var_a0) != 0) {
                    break;
                }
                var_a2_old = g_MenuLevelDisplayValue;
            }
        } else {
            var_a3 = (u8 *)&g_AyaStatAgility;
            var_a2 = 0;
            var_a1 = (u8 *)&g_BonusPointStatMultipliers;
            var_a0_2 = (u8 *)&g_BonusPointStatDeltas;
            do {
                temp_v1 = M2C_FIELD(var_a1, s32 *, 0);
                var_a1 += 4;
                temp_v0 = M2C_FIELD(var_a0_2, s32 *, 0);
                var_a0_2 += 4;
                var_a2 += 1;
                temp_v0 += temp_v1;
                M2C_FIELD(var_a3, s16 *, 0) = temp_v0;
                var_a3 += 2;
            } while (var_a2 < 9);
            temp_v0 = g_MenuLevelDisplayValue;
            temp_v1 = g_MenuHpMaxDisplayValue;
            temp_bonus = g_BonusPointDisplayValue;
            g_AyaSaveLevel = (s8) temp_v0;
            g_AyaHpMax = (s16) temp_v1;
            g_AyaBonusPoints = temp_bonus;
            Inv_RecalcSlotStats(temp_bonus, var_a1, var_a2, var_a3);
            MenuWidget_DestroyNode(arg0);
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x1E));
            MenuWidget_NavScrollTo(0x1C);
            Akao_FlushBgmVolumeFade();
            var_a0 = 0xA;
            if (Menu_GetBattleCount() != 0) {
                Menu_StepSaveLoadScreen();
            } else {
                Inv_SetActiveList(0xA, 0);
            }
        }
        Menu_PlayConfirmSound();
    }
    return 1;
}
