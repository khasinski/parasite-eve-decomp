#include "common.h"
#include "pe1/menu_inventory.h"
#include "pe1/aya.h"
#include "pe1/psyq_nop.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void Menu_CreateBonusPointAllocationView(void);

s32 Menu_GetBattleEquipMode();                                /* extern */
extern s32 g_MenuItemContextFlag;
extern s32 g_MenuBattleStatusOverlayActive;
extern s32 g_MenuSelectionLocked;
int Menu_StepItemSelectScreen(MenuWidgetNode *node, unsigned int flags);
void Menu_DrawItemList(void *node);

void Menu_CreateInventoryTabView(void) {
    s32 var_a0;
    s32 var_a1;
    s32 var_v1;
    MenuWidgetNode *temp_a0;
    MenuWidgetNode *temp_s1;
    MenuWidgetNode *temp_v0;

    if (MenuWidget_FindByModeAndSelectedBase(1, 0) == 0) {
        temp_v0 = MenuWidget_CreateSimpleNode(0, 0, 0, 0);
        temp_s1 = MenuWidget_CreateNode(0, temp_v0, temp_v0);
        temp_a0 = temp_s1;
        temp_v0->update = (void (*)())Menu_StepItemSelectScreen;
        temp_s1->draw = (void (*)())Menu_DrawItemList;
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
extern u8 D_80092258[];
extern u8 D_80092298[];
extern s32 g_BonusPointDisplayValue;
extern int g_BonusPointStatQueryResults[];
extern int g_BonusPointStatDeltas[];
extern int g_BonusPointStatMultipliers[];
extern u16 g_AyaStatAgility[];
#define g_AyaStatAgility (g_AyaStatAgility[0])
void Menu_DrawStatusPanel(void);
void Menu_DrawStatsList(void);
void Menu_DrawBonusPointSlotValue(void);
extern s32 g_AyaBonusPoints[];
#define g_AyaBonusPoints (g_AyaBonusPoints[0])

void Menu_CreateBonusPointAllocationView(void);

void Menu_CreateBonusPointAllocationView(void) {
    register int *temp_a2 asm("$6");
    int *var_s3;
    int *var_s0;
    int *var_s2;
    register s32 temp_a0 asm("$4");
    int *temp_a3;
    s32 temp_a1;
    s32 var_s1;
    u16 *var_s4;
    u16 temp_v0_3;
    s32 temp_v0_final;
    MenuWidgetNode *temp_v0;
    MenuWidgetNode *temp_v0_2;
    register MenuWidgetNode *temp_v1_reg asm("$3");

    temp_v0 = MenuWidget_CreateSimpleNode(0x12, 0, 0, 0);
    temp_v1_reg = temp_v0;
    temp_v1_reg->draw = Menu_DrawStatusPanel;
    temp_v1_reg->appearance.gradientPoints = D_80092258;
    ((MenuWidgetNode *)MenuWidget_CreateSimpleNode(0x18, 0, 0, 0))->draw =
        Menu_DrawBonusPointSlotValue;
    temp_v0_2 = MenuWidget_CreateSimpleNode(0x2D, 0, 0, 0);
    temp_v1_reg = temp_v0_2;
    var_s4 = &g_AyaStatAgility;
    var_s1 = 0;
    var_s0 = g_BonusPointStatDeltas;
    var_s3 = g_BonusPointStatQueryResults;
    var_s2 = g_BonusPointStatMultipliers;
    temp_v1_reg->draw = Menu_DrawStatsList;
    temp_v1_reg->appearance.gradientPoints = D_80092298;
    do {
        temp_v0_3 = *var_s4;
        var_s4 += 1;
        temp_a0 = var_s1;
        temp_a2 = var_s3;
        temp_a3 = 0;
        __asm__ volatile("" : "=r"(temp_a3) : "0"(temp_a3));
        var_s3 += 1;
        var_s1 += 1;
        *var_s0 = temp_v0_3;
        *var_s2 = 0;
        temp_a1 = *var_s0;
        var_s0 += 1;
        var_s2 += 1;
        Stat_QueryLevelAndSubLevel(temp_a0, temp_a1, temp_a2, temp_a3);
    } while (var_s1 < 7);
    temp_v0_final = g_AyaBonusPoints;
    PE1_NOP();
    g_BonusPointDisplayValue = temp_v0_final;
}
