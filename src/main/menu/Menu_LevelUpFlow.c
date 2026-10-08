#include "pe1/menu_bonus_stats.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "common.h"
#include "pe1/menu_inventory_root.h"
#include "pe1/menu_state.h"
#include "pe1/menu_context_help.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_modifiers.h"
#include "pe1/psyq_nop.h"
#include "../../../tools/m2c/m2c_macros.h"

void *Aya_GetLevelExpTable(void);
s32 Stat_BinarySearch(s32 value, void *table);
s32 Battle_GetScaledMaxHP(s32 level);
void Akao_SetBgmVolumeFade(void);
void Akao_FlushBgmVolumeFade(void);
void Menu_ResetInputState(void);
void Menu_CreateExpReviewView(void);
void Menu_DrawExpReviewPanel(void);
s32 Menu_ConfirmExpAllocation(MenuWidgetNode *node, s32 flags);
void Menu_CreateLevelUpBanner(void);
void Menu_DrawLevelUpBanner(void);
void Menu_CreateLevelUpResultPanel(void);
s32 Aya_UnlockParasiteSpellById(s32 spell);
void Menu_UpdateStatBarAnimation(s32 stat);
void Draw_PrintNumberWidth2(s32 value);
void Draw_PrintNumberWidth5(s32 value);
void Draw_PrintNumberWidth6(int value);

extern int g_MenuLevelDisplayValue, g_MenuLevelDisplayTarget;
extern int g_MenuHpMaxDisplayValue, g_MenuHpMaxDisplayTarget;
extern int g_BonusPointDisplayValue, g_MenuBonusPointDisplayTarget;
extern int g_BonusPointBarAnimStep, g_BonusPointBarAnimProgress;
extern int g_MenuBonusPointBarAnimActive;
extern u8 D_800922F4[];
extern s32 g_AyaSaveTotalExp[];
#define g_AyaSaveTotalExp (g_AyaSaveTotalExp[0])
/* Distinct views preserve the original loads and store around the bonus update. */
extern u16 D1E_read[16] __asm__("D_800C0E1E");
extern u16 D1E_write[16] __asm__("D_800C0E1E");
extern u16 D1E_bonus[16] __asm__("D_800C0E1E");
/* This loop sign-extends the stat allocations (lh in the original). */
extern s16 D_800C0E28_signed[7] __asm__("D_800C0E28");

extern AyaSaveState D_800C0E00_array[] __asm__("D_800C0E00");
extern u8 aya_level_view[16] __asm__("D_800C0E0A");
extern u16 aya_max_hp_view[16] __asm__("D_800C0E06");
extern u32 aya_bonus_view[4] __asm__("D_800C0E10");

void Aya_SetTotalExp(s32 exp_delta, s32 pe_bonus_delta, void *wayne_items) {
    s32 exp_delta_reg;
    s32 pe_bonus_delta_reg;
    void *wayne_items_reg;
    AyaSaveState *save;
    s32 previous_exp;
    s16 *stat_src;
    s32 i;
    s32 stat_level;

    exp_delta_reg = exp_delta;
    pe_bonus_delta_reg = pe_bonus_delta;
    wayne_items_reg = wayne_items;

    Menu_SetBattleEquipMode(0);
    BattleCmd_SyncActiveAmmo();

    save = &D_800C0E00_array[0];
    asm volatile("" : "=r"(save) : "0"(save));
    previous_exp = save->total_exp;
    PE1_NOP();
    g_MenuPendingTotalExp = previous_exp;
    asm volatile("" : : : "memory");
    g_MenuExpAllocTarget = previous_exp + exp_delta_reg;
    g_MenuExpReviewLevel = Stat_BinarySearch(g_MenuPendingTotalExp, Aya_GetLevelExpTable());
    D_8009CEFC = 1;

    D1E_write[0] = D1E_read[0] + pe_bonus_delta_reg;
    g_MenuLevelDisplayValue = aya_level_view[0];
    g_MenuHpMaxDisplayValue = aya_max_hp_view[0];
    g_MenuBonusPointDisplayTarget = aya_bonus_view[0];
    g_BonusPointDisplayValue = aya_bonus_view[0];
    g_MenuLevelDisplayTarget = Stat_BinarySearch(g_MenuExpAllocTarget, Aya_GetLevelExpTable());

    if (g_MenuLevelDisplayValue < g_MenuLevelDisplayTarget) {
        s32 cap = 0x1869F;
        register s32 result asm("$5") = 0x1869F;
        s32 bonus = g_MenuBonusPointDisplayTarget + D1E_bonus[0];
        if (bonus <= cap) result = bonus;
        g_MenuBonusPointDisplayTarget = result;
        save->pe_bonus_pool = 0;
    }

    stat_src = D_800C0E28_signed;
    i = 0;
    do {
        g_BonusPointStatDeltas[i] = *stat_src++;
        Stat_QueryLevelAndSubLevel(i, g_BonusPointStatDeltas[i], &D_800A18B4.queryResults[i], 0);
        g_BonusPointStatMultipliers[i] = (g_MenuLevelDisplayTarget - g_MenuLevelDisplayValue) * 10;
        i++;
    } while (i < 7);

    Stat_QueryLevelAndSubLevel(0, g_BonusPointStatDeltas[0] + g_BonusPointStatMultipliers[0], &stat_level, 0);
    g_MenuHpMaxDisplayTarget = Battle_GetScaledMaxHP(stat_level);
    g_BonusPointBarAnimStep = 2;
    Inv_LoadWayneItemsAsOverride(wayne_items_reg);
    Menu_CreateExpReviewView();
    Akao_SetBgmVolumeFade();
    Menu_ResetInputState();
}

void Menu_CreateExpReviewView(void) {
    MenuWidgetNode *node;

    node = MenuWidget_CreateSimpleNode(0x14, 0, 0, 0);
    node->draw = Menu_DrawExpReviewPanel;
    node->update = Menu_ConfirmExpAllocation;
    MenuWidget_SetCurrentNode(node);
    node->appearance.gradientPoints = D_800922F4;
}

void Menu_DrawExpReviewPanel(void) {
    s32 can_step;
    s32 level;
    s32 exp_to_next;

    can_step = g_MenuPendingTotalExp < g_MenuExpAllocTarget;
    if (!can_step) {
        Akao_FlushBgmVolumeFade();
    }

    level = Stat_BinarySearch(g_MenuPendingTotalExp, Aya_GetLevelExpTable());
    if (level < 0x62) {
        exp_to_next = ((s32 *)Aya_GetLevelExpTable())[level + 1] - g_MenuPendingTotalExp;
    } else {
        exp_to_next = 0;
    }

    g_MenuPendingTotalExp += can_step;
    if (g_MenuExpReviewLevel < level) {
        g_MenuExpReviewLevel = level;
        g_MenuLevelUpAnimTimer = 0x3C;
        if (MenuWidget_FindByModeAndSelectedBase(1, 0x16) == 0) {
            Menu_CreateLevelUpBanner();
        }
    }

    if (g_MenuLevelUpAnimTimer != 0) {
        g_MenuLevelUpAnimTimer--;
        if (g_MenuLevelUpAnimTimer == 0) {
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x16));
        }
    }

    Draw_OffsetCursor(4, 4);
    Draw_StatePush();
    Draw_PrintRawText(Str_LookupTable4(9));
    Draw_OffsetCursor(0x50, 0);
    Draw_PrintNumberWidth6(
        0xF423F < g_MenuPendingTotalExp ? 0xF423F : g_MenuPendingTotalExp);
    Draw_OffsetCursor(2, 2);
    Draw_AllocSprite(0x8A);
    Draw_StatePop();

    Draw_OffsetCursor(0, 0xE);
    Draw_StatePush();
    Draw_PrintRawText(Str_LookupTable4(0xA));
    Draw_OffsetCursor(0x50, 0);
    Draw_PrintNumberWidth6(exp_to_next);
    Draw_OffsetCursor(2, 2);
    Draw_AllocSprite(0x8A);
    Draw_StatePop();

    Draw_OffsetCursor(0, 0x14);
    Draw_StatePush();
    Draw_PrintRawText(Str_LookupTable4(0xC));
    Draw_OffsetCursor(0x74, 0);
    Draw_PrintNumberWidth2(Menu_GetBattleCount());
    Draw_StatePop();
}


s32 Menu_ConfirmExpAllocation(MenuWidgetNode *node, s32 flags) {
    s32 confirm;
    s32 new_level;

    confirm = flags & 0x10000;
    if (confirm != 0) {
        if (g_MenuPendingTotalExp < g_MenuExpAllocTarget) {
            g_MenuPendingTotalExp = g_MenuExpAllocTarget;
        } else {
            MenuWidget_DestroyNode(node);
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x16));
            Akao_FlushBgmVolumeFade();
            new_level = Stat_BinarySearch(g_MenuPendingTotalExp, Aya_GetLevelExpTable());
            if (new_level != Stat_BinarySearch(g_AyaSaveTotalExp, Aya_GetLevelExpTable())) {
                Menu_CreateLevelUpResultPanel();
                Akao_SetBgmVolumeFade();
            } else if (Menu_GetBattleCount() != 0) {
                Menu_StepSaveLoadScreen();
            } else {
                Inv_SetActiveList(0xA, 0);
            }
            g_AyaSaveTotalExp = g_MenuPendingTotalExp;
        }
        Menu_PlayConfirmSound();
    }
    return 1;
}

void Menu_CreateLevelUpBanner(void) {
    *(void **)((char *)MenuWidget_CreateSimpleNode(0x16, 0, 0, 0) + 0x30) = Menu_DrawLevelUpBanner;
}

void Menu_DrawLevelUpBanner(void) {
    Draw_OffsetCursor(0, 4);
    Draw_PrintCenteredText(Str_LookupTable4(0x19));
}

void Menu_InitBonusPointAllocState(int gained_points) {
    int scratch[2];
    s16 *source;
    int *deltas;
    int *query_results;
    int *multipliers;
    int stat;
    int value;

    BattleCmd_SyncActiveAmmo();

    g_MenuPendingTotalExp = D_800C0E00.total_exp;
    g_MenuExpAllocTarget = D_800C0E00.total_exp;
    g_MenuLevelDisplayValue = D_800C0E00.level;
    g_MenuExpReviewLevel = D_800C0E00.level;
    g_MenuLevelDisplayTarget = D_800C0E00.level;
    g_MenuHpMaxDisplayValue = D_800C0E00.max_hp;
    g_MenuHpMaxDisplayTarget = D_800C0E00.max_hp;
    g_BonusPointDisplayValue = D_800C0E00.bonus_points;

    if (gained_points < 0) {
        gained_points = 0;
    }
    g_MenuBonusPointDisplayTarget = D_800C0E00.bonus_points + gained_points;

    source = (s16 *)D_800C0E00.stats.levels;
    stat = 0;
    multipliers = g_BonusPointStatMultipliers;
    query_results = g_BonusPointStatQueryResults;
    deltas = g_BonusPointStatDeltas;
    do {
        value = *source++;
        *deltas = value;
        Stat_QueryLevelAndSubLevel(stat, value, query_results, 0);
        *multipliers = 0;
        multipliers++;
        query_results++;
        deltas++;
        stat++;
    } while (stat < 7);

    Stat_QueryLevelAndSubLevel(0, g_BonusPointStatDeltas[0], scratch, 0);
    g_BonusPointBarAnimStep = 2;
    Inv_LoadWayneItemsAsOverride(0);
    Menu_CreateLevelUpResultPanel();
    MenuWidget_OffsetPosition(MenuWidget_FindByModeAndSelectedBase(1, 0x15), 0, 0x20);
    if (gained_points > 0) {
        Akao_SetBgmVolumeFade();
    }
    Menu_ResetInputState();
}


void Menu_DrawBonusPointAnimFrame(void);
s32 Menu_StepParasiteScreen(MenuWidgetNode *node, s32 flags);
void Menu_DrawStatAllocationList();
extern u8 D_80092314[];

void Menu_CreateLevelUpResultPanel(void) {
    MenuWidgetNode *root;
    MenuWidgetNode *child;

    root = MenuWidget_CreateSimpleNode(0x15, 0, 0, 0);
    child = MenuWidget_CreateNode(0x2F, root, root);
    root->draw = Menu_DrawBonusPointAnimFrame;
    root->update = Menu_StepParasiteScreen;
    child->draw = Menu_DrawStatAllocationList;
    child->cursor_x = -1;
    child->x += 0x44;
    child->y += 2;
    child->disabled -= 2;
    MenuWidget_SetCurrentNode(root);
    root->appearance.gradientPoints = D_80092314;
    g_MenuBonusPointBarAnimActive = 1;
    Menu_InitStateTables();
}


extern int g_MenuStatBarWidths[8];
extern int g_MenuStatBarSettleTimers[8];

void Menu_InitStateTables(void) {
    int i;

    for (i = 0; i < 8; i++) {
        g_MenuStatBarSettleTimers[i] = 0;
        g_MenuStatBarWidths[i] = 0;
    }
}

extern s32 D_8009CF7C;
extern u16 g_AyaHpMax[];
#define g_AyaHpMax (g_AyaHpMax[0])
extern s32 g_AyaBonusPoints[];
#define g_AyaBonusPoints (g_AyaBonusPoints[0])
extern u8 g_AyaSaveLevel[];
#define g_AyaSaveLevel (g_AyaSaveLevel[0])

void Menu_DrawBonusPointAnimFrame(void) {
    M2C_UNK var_a0;
    M2C_UNK var_a0_2;
    M2C_UNK var_a0_3;
    s32 temp_a0;
    s32 temp_v0;
    s32 temp_v1;
    s32 var_s0;
    s32 var_s0_2;

    Draw_OffsetCursor(4, 4);
    Draw_AllocSprite(0x97);
    var_s0 = 0;
    if (g_MenuLevelDisplayValue < g_MenuLevelDisplayTarget) {
        temp_v0 = g_MenuLevelUpAnimTimer < 0x1E;
        g_MenuLevelUpAnimTimer += 1;
        var_s0 = 1;
        if (temp_v0 == 0) {
            temp_a0 = g_MenuLevelDisplayValue + 1;
            g_MenuLevelUpAnimTimer = 0;
            g_MenuLevelDisplayValue = temp_a0;
            Aya_UnlockParasiteSpellById(temp_a0);
        }
    }
    var_a0 = 0x808080;
    if (g_AyaSaveLevel < g_MenuLevelDisplayValue) {
        var_a0 = 0x8080;
    }
    Draw_SetColor(var_a0);
    Draw_OffsetCursor(0xF, 0x10);
    Draw_PrintNumberWidth5(g_MenuLevelDisplayValue + 1);
    Draw_SetColor(0x808080);
    Draw_OffsetCursor(-0x3C, 0x14);
    Draw_AllocSprite(0x99);
    if (g_MenuHpMaxDisplayValue < g_MenuHpMaxDisplayTarget) {
        temp_v1 = D_8009CF7C;
        D_8009CF7C = temp_v1 + 1;
        var_s0 = 1;
        if (temp_v1 >= 0) {
            D_8009CF7C = 0;
            g_MenuHpMaxDisplayValue += 1;
        }
    }
    var_a0_2 = 0x808080;
    if (g_AyaHpMax < g_MenuHpMaxDisplayValue) {
        var_a0_2 = 0x8080;
    }
    Draw_SetColor(var_a0_2);
    Draw_OffsetCursor(0xF, 0x10);
    Draw_PrintNumberWidth5(g_MenuHpMaxDisplayValue);
    Draw_OffsetCursor(-0x3C, 0x13);
    Draw_SetColor(0x808080);
    Draw_AllocSprite(0x93);
    if (g_BonusPointDisplayValue < g_MenuBonusPointDisplayTarget) {
        g_BonusPointDisplayValue += 1;
        var_s0 = 1;
    }
    temp_v1 = g_BonusPointDisplayValue;
    var_a0_3 = 0x808080;
    if (g_AyaBonusPoints < temp_v1) {
        var_a0_3 = 0x8080;
    }
    Draw_SetColor(var_a0_3);
    Draw_OffsetCursor(0xF, 0x11);
    Draw_PrintNumberWidth5(g_BonusPointDisplayValue);
    Draw_OffsetCursor(0xA, -0x56);
    Draw_SetColor(0x808080);
    if (g_MenuBonusPointBarAnimActive != 0) {
        if (g_BonusPointBarAnimProgress < 0x80) {
            g_BonusPointBarAnimProgress += g_BonusPointBarAnimStep;
        }
        if ((var_s0 == 0) && (g_BonusPointBarAnimProgress >= 0x80)) {
            g_BonusPointBarAnimProgress = 0x80;
            g_MenuBonusPointBarAnimActive = 0;
        }
    }
    Draw_OffsetCursor(0x78, 2);
    var_s0_2 = 1;
    do {
        Menu_UpdateStatBarAnimation(var_s0_2);
        Draw_OffsetCursor(0, 0x10);
        var_s0_2 += 1;
    } while (var_s0_2 < 7);
    if (g_MenuBonusPointBarAnimActive == 0) {
        Akao_FlushBgmVolumeFade();
    }
}

s32 Menu_StepParasiteScreen(MenuWidgetNode *arg0, s32 arg1) {
    u16 *statDestination;
    s32 *statDeltas;
    s32 *statMultipliers;
    s32 temp_v0;
    register s32 temp_v1 asm("$3");
    s32 value;
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
                value = Aya_UnlockParasiteSpellById(var_a0);
                if (value != 0) {
                    break;
                }
                var_a2_old = g_MenuLevelDisplayValue;
            }
        } else {
            statDestination = D_800C0E00.stats.transfer;
            var_a2 = 0;
            statMultipliers = g_BonusPointStatMultipliers;
            statDeltas = g_BonusPointStatDeltas;
            do {
                temp_v1 = *statMultipliers;
                statMultipliers++;
                temp_v0 = *statDeltas;
                statDeltas++;
                var_a2 += 1;
                temp_v0 += temp_v1;
                *statDestination = temp_v0;
                statDestination++;
            } while (var_a2 < 9);
            temp_v0 = g_MenuLevelDisplayValue;
            temp_v1 = g_MenuHpMaxDisplayValue;
            value = g_BonusPointDisplayValue;
            g_AyaSaveLevel = (s8) temp_v0;
            g_AyaHpMax = (s16) temp_v1;
            g_AyaBonusPoints = value;
            Inv_RecalcSlotStats();
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
