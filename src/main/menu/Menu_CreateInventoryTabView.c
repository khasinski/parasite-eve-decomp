#include "common.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 MenuWidget_FindByModeAndSelectedBase(M2C_UNK, M2C_UNK); /* extern */
M2C_UNK MenuWidget_SetCurrentNode(void *);          /* extern */
void Menu_CreateBonusPointAllocationView(void);
M2C_UNK Menu_CreateContextHelpPanel();                            /* extern */
s32 Menu_GetBattleEquipMode();                                /* extern */
void *MenuWidget_CreateSimpleNode(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK); /* extern */
void *MenuWidget_CreateNode(M2C_UNK, void *, void *);       /* extern */
extern s32 g_MenuItemContextFlag;
extern s32 g_MenuBattleStatusOverlayActive;
extern s32 g_MenuSelectionLocked;
extern M2C_UNK Menu_StepItemSelectScreen[];
#define Menu_StepItemSelectScreen (Menu_StepItemSelectScreen[0])
extern M2C_UNK Menu_DrawItemList[];
#define Menu_DrawItemList (Menu_DrawItemList[0])

void Menu_CreateInventoryTabView(void) {
    s32 var_a0;
    s32 var_a1;
    s32 var_v1;
    void *temp_a0;
    void *temp_s1;
    void *temp_v0;

    if (MenuWidget_FindByModeAndSelectedBase(1, 0) == 0) {
        temp_v0 = MenuWidget_CreateSimpleNode(0, 0, 0, 0);
        temp_s1 = MenuWidget_CreateNode(0, temp_v0, temp_v0);
        temp_a0 = temp_s1;
        M2C_FIELD(temp_v0, M2C_UNK **, 0x2C) = &Menu_StepItemSelectScreen;
        M2C_FIELD(temp_s1, M2C_UNK **, 0x30) = &Menu_DrawItemList;
        MenuWidget_SetCurrentNode(temp_a0);
        if (Menu_GetBattleEquipMode() != 0) {
            var_v1 = g_MenuItemContextFlag & 0x1F;
        } else {
            var_v1 = g_MenuItemContextFlag & 0x1EF;
        }
        var_a1 = 0;
        var_a0 = 8;
        do {
            var_a1 += var_v1 & 1;
            var_a0 -= 1;
            var_v1 = var_v1 >> 1;
        } while (var_a0 >= 0);
        Draw_SetPrimCallback(temp_s1, var_a1);
        g_MenuSelectionLocked = 0;
        g_MenuBattleStatusOverlayActive = 1;
        Menu_CreateBonusPointAllocationView();
        Menu_CreateContextHelpPanel();
    }
}

#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Stat_QueryLevelAndSubLevel();
void *MenuWidget_CreateSimpleNode();
extern M2C_UNK D_80092258[];
#define D_80092258 (D_80092258[0])
extern M2C_UNK D_80092298[];
#define D_80092298 (D_80092298[0])
extern s32 g_BonusPointDisplayValue;
extern M2C_UNK g_BonusPointStatQueryResults[];
#define g_BonusPointStatQueryResults (g_BonusPointStatQueryResults[0])
extern s32 g_BonusPointStatDeltas[];
#define g_BonusPointStatDeltas (g_BonusPointStatDeltas[0])
extern s32 g_BonusPointStatMultipliers[];
#define g_BonusPointStatMultipliers (g_BonusPointStatMultipliers[0])
extern u16 g_AyaStatAgility[];
#define g_AyaStatAgility (g_AyaStatAgility[0])
extern M2C_UNK Menu_DrawStatusPanel[];
#define Menu_DrawStatusPanel (Menu_DrawStatusPanel[0])
extern M2C_UNK Menu_DrawStatsList[];
#define Menu_DrawStatsList (Menu_DrawStatsList[0])
extern M2C_UNK Menu_DrawBonusPointSlotValue[];
#define Menu_DrawBonusPointSlotValue (Menu_DrawBonusPointSlotValue[0])
extern s32 g_AyaBonusPoints[];
#define g_AyaBonusPoints (g_AyaBonusPoints[0])

void Menu_CreateBonusPointAllocationView(void);

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

    temp_v0 = MenuWidget_CreateSimpleNode(0x12, 0, 0, 0);
    temp_v1_reg = temp_v0;
    M2C_FIELD(temp_v1_reg, M2C_UNK **, 0x30) = &Menu_DrawStatusPanel;
    M2C_FIELD(temp_v1_reg, M2C_UNK **, 0x4C) = &D_80092258;
    M2C_FIELD(MenuWidget_CreateSimpleNode(0x18, 0, 0, 0), M2C_UNK **, 0x30) = &Menu_DrawBonusPointSlotValue;
    temp_v0_2 = MenuWidget_CreateSimpleNode(0x2D, 0, 0, 0);
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
