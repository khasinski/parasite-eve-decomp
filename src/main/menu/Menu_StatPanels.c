#include "pe1/menu_state.h"
#include "pe1/menu_bonus_stats.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/menu_inventory.h"
#include "pe1/aya.h"
#include "pe1/psyq_nop.h"
#include "../../../tools/m2c/m2c_macros.h"
#include "pe1/save.h"
#include "pe1/draw_state.h"
#include "pe1/text.h"
#include "pe1/stat_modifiers.h"

/* Inventory stat-panel setup and its adjacent draw callbacks. */


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


extern u8 D_80092258[];
extern u8 D_80092298[];
extern s32 g_BonusPointDisplayValue;
void Menu_DrawStatusPanel(void);
void Menu_DrawStatsList(void);
void Menu_DrawBonusPointSlotValue(void);


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
    MenuWidgetNode *temp_v1_reg;

    temp_v0 = temp_v1_reg = MenuWidget_CreateSimpleNode(0x12, 0, 0, 0);
    temp_v1_reg->draw = Menu_DrawStatusPanel;
    temp_v1_reg->appearance.gradientPoints = D_80092258;
    ((MenuWidgetNode *)MenuWidget_CreateSimpleNode(0x18, 0, 0, 0))->draw =
        Menu_DrawBonusPointSlotValue;
    temp_v0_2 = temp_v1_reg = MenuWidget_CreateSimpleNode(0x2D, 0, 0, 0);
    var_s4 = &D_800C0E00.stats.levels[0];
    var_s1 = 0;
    var_s0 = g_BonusPointStatDeltas;
    var_s3 = g_BonusPointStatQueryResults;
    var_s2 = g_BonusPointStatMultipliers;
    temp_v1_reg->draw = Menu_DrawStatsList;
    temp_v1_reg->appearance.gradientPoints = D_80092298;
    for (; var_s1 < 7; ++var_s1) {
        temp_v0_3 = *var_s4;
        var_s4 += 1;
        temp_a0 = var_s1;
        temp_a2 = var_s3;
        temp_a3 = 0;
        __asm__ volatile("" : "=r"(temp_a3) : "0"(temp_a3));
        var_s3 += 1;
        *var_s0 = temp_v0_3;
        *var_s2 = 0;
        temp_a1 = *var_s0;
        var_s0 += 1;
        var_s2 += 1;
        Stat_QueryLevelAndSubLevel(temp_a0, temp_a1, temp_a2, temp_a3);
    }
    temp_v0_final = D_800C0E00.bonus_points;
    PE1_NOP();
    g_BonusPointDisplayValue = temp_v0_final;
}

void Sfx_DrawActiveListSlot(int slot);
void *Aya_GetLevelExpTable(void);
void Draw_PrintRawText(u8 *text);
void Draw_PrintNumberWidth6(int value);
void Draw_PrintNumberWidth3(int value);

void Menu_DrawStatusPanel(void) {
    s32 var_a0;

    Draw_OffsetCursor(2, 2);
    Draw_AllocSprite(0x47);
    Draw_OffsetCursor(0x28, 2);
    Draw_PrintRawText(Save_GetActiveMetadataBuffer());
    Draw_OffsetCursor(0, 0x15);
    Draw_AllocSprite(0x97);
    Draw_OffsetCursor(0x3C, 0);
    Draw_PrintNumberWidth3(D_800C0E00.level + 1);
    Draw_OffsetCursor(-0x78, 0x1A);
    Draw_AllocSprite(0x98);
    Draw_OffsetCursor(0x42, 0);
    if ((u8) D_800C0E00.level < 0x62U) {
        var_a0 = ((s32 *)Aya_GetLevelExpTable())[D_800C0E00.level + 1] - D_800C0E00.total_exp;
    } else {
        var_a0 = 0;
    }
    Draw_PrintNumberWidth6(var_a0);
    Draw_OffsetCursor(-0x7C, 0x15);
    Draw_AllocSprite(0x94);
    Draw_OffsetCursor(0, 0x10);
    Sfx_DrawActiveListSlot(D_800C0E00.equipped_weapon_slot);
    Draw_OffsetCursor(0, 0x10);
    if (D_800C0E00.equipped_armor_slot >= 0) {
        Sfx_DrawActiveListSlot(D_800C0E00.equipped_armor_slot);
        return;
    }
    Draw_PrintTextById(0x39);
}


void Draw_PrintNumberWidth2(int value);
extern s32 g_BonusPointBarAnimProgress;
extern s32 D_800A18DC[];
#define D_800A18DC (D_800A18DC[0])
extern s32 g_BonusPointStatMultipliers[];

void Menu_DrawStatsList(void) {
    s32 sp10;
    M2C_UNK var_a0;
    M2C_UNK var_a1;
    s32 *var_s3;
    register s32 *var_s4 asm("$20");
    s32 temp_s0;
    s32 temp_s1;
    register s32 var_a0_2 asm("$4");
    s32 var_s2;

    var_s4 = Battle_GetModifierTable() + 1;
    Draw_OffsetCursor(4, 5);
    var_s2 = 1;
    var_s3 = &D_800A18DC;
    do {
        temp_s1 = *var_s4;
        temp_s0 = *var_s3 + ((g_BonusPointBarAnimProgress * g_BonusPointStatMultipliers[var_s2]) >> 7);
        Draw_OffsetCursor(2, 0);
        Draw_AllocSprite(var_s2 + 0x8C);
        Draw_OffsetCursor(0x4A, 0);
        Stat_QueryLevelAndSubLevel(var_s2, temp_s0, &sp10, 0);
        var_s4 += 1;
        Draw_PrintNumberWidth2(sp10 + 1);
        if (temp_s1 != 0) {
            Draw_OffsetCursor(6, 0);
            var_a0 = 0x70;
            if (temp_s1 > 0) {
                var_a0 = 0x6F;
            }
            Draw_PrintTextById(var_a0);
            var_a0_2 = temp_s1;
            if (temp_s1 < 0) {
                var_a0_2 = -var_a0_2;
            }
            Draw_PrintNumberWidth2(var_a0_2);
            Draw_OffsetCursor(-0x18, 0);
        }
        var_a1 = 0xE;
        if (var_s2 == 4) {
            var_a1 = 0x16;
        }
        Draw_OffsetCursor(-0x5E, var_a1);
        var_s2 += 1;
        var_s3 += 1;
    } while (var_s2 < 7);
}
