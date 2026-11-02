/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/akao_counters.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_modifiers.h"
#include "pe1/draw_state.h"
#include "pe1/draw_wipe_rect.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_bonus_stats.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_inventory_root.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/psyq_gpu.h"
#include "pe1/text.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_MenuPendingTotalExp;
int g_MenuSelectionLocked;
int g_AyaFullInvHold;
int g_InvSelectedItemIndex;
int g_InvSwapTargetIndex;
int g_MenuActionSubmenuOpen;
int g_MenuBlendColorBackup;
int g_MenuEquipSwapSource;

int Menu_MainMenuInputHandler(MenuWidgetNode *arg0, u32 arg1);
void Menu_OpenCursorModeMenu(MenuWidgetNode *parent);
int Menu_ActionSubmenuInputHandler(MenuWidgetNode *arg0, u32 arg1);
int Menu_CursorModeInputHandler(MenuWidgetNode *arg0, u32 arg1);

void Menu_CreateMainMenuView(MenuWidgetNode *arg0) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;

    parent = MenuWidget_CreateSimpleNode(0x20, arg0, 0, 0);
    child = MenuWidget_CreateNode(0x20, parent, parent);
    parent->update = Menu_MainMenuInputHandler;
    child->draw = Menu_DrawMainMenuList;
    MenuWidget_SetCurrentNode(child);
    Draw_SetPrimCallback(child, 4);
}

int Menu_MainMenuInputHandler(MenuWidgetNode *arg0, u32 arg1) {
    MenuWidgetNode *child = MenuWidget_GetChild(arg0, 0);

    if (arg1 & 0x10000) {
        switch (MenuWidget_GridCellIndex(child)) {
        case 0:
            Menu_CreateActionSubmenu(child);
            break;
        case 1:
            Menu_OpenCursorModeMenu(child);
            break;
        case 2:
            Menu_OpenBlendColorScreen(child);
            break;
        case 3:
            Menu_OpenScreenAdjustView(child);
            break;
        case 4:
        case 5:
            Inv_InitMaxLevelInventory(MenuWidget_GridCellIndex(child) - 4);
            MenuWidget_DestroyNode(arg0);
            Menu_CreateBonusPointAllocationView();
            Menu_PlayConfirmSound();
            break;
        }
        Menu_PlayConfirmSound();
        return 1;
    }

    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_CreateBonusPointAllocationView();
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_CreateActionSubmenu(MenuWidgetNode *arg0) {
    MenuWidgetNode *created;
    MenuWidgetNode *node;

    created = MenuWidget_CreateSimpleNode(0x21, arg0, 0, 0);
    node = MenuWidget_CreateNode(0x21, created, created);
    created->update = Menu_ActionSubmenuInputHandler;
    node->draw = Menu_DrawActionSubmenuList;
    MenuWidget_SetCurrentNode(node);
}

int Menu_ActionSubmenuInputHandler(MenuWidgetNode *arg0, u32 arg1) {
    MenuWidgetNode *base;

    base = MenuWidget_GetChild(arg0, 0);
    if (arg1 & 0x10000) {
        Menu_PlayConfirmSound();
        Menu_SetActionSubmenuSelection(MenuWidget_GridCellIndex(base));
        MenuWidget_DestroyNode(arg0);
        return 1;
    }
    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_OpenCursorModeMenu(MenuWidgetNode *arg0) {
    MenuWidgetNode *created;
    MenuWidgetNode *node;

    created = MenuWidget_CreateSimpleNode(0x23, arg0, 0, 0);
    node = MenuWidget_CreateNode(0x23, created, created);
    created->update = Menu_CursorModeInputHandler;
    node->draw = Menu_DrawCursorModeList;
    MenuWidget_SetCurrentNode(node);
}

int Menu_CursorModeInputHandler(MenuWidgetNode *arg0, u32 arg1) {
    MenuWidgetNode *base;

    base = MenuWidget_GetChild(arg0, 0);
    if (arg1 & 0x10000) {
        Menu_PlayConfirmSound();
        MenuWidget_SetColumnLayoutMode(MenuWidget_GridCellIndex(base));
        MenuWidget_DestroyNode(arg0);
        return 1;
    }
    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_DrawBlendColorArrows(MenuWidgetNode *panel);
s32 Menu_StepColorSelect(MenuWidgetNode *panel, s32 flags);
void Menu_DrawBlendColorChannelList(int node);
void Menu_DrawBlendColorOptionList(int node);
void Menu_DrawScreenAdjustPanel(void);
int Menu_ScreenAdjustInputHandler(void *node, int input);

void Menu_OpenBlendColorScreen(MenuWidgetNode *arg0) {
    MenuWidgetNode *parent;
    MenuWidgetNode *panel;
    MenuWidgetNode *options;

    parent = MenuWidget_CreateSimpleNode(0x2E, arg0, 0, 0);
    panel = MenuWidget_CreateNode(0x2E, parent, parent);
    parent->draw = Menu_DrawBlendColorArrows;
    parent->update = Menu_StepColorSelect;
    parent->appearance.gradientPoints = g_MenuBlendColorPanelPoints;
    parent->disabled = 1;
    panel->draw = Menu_DrawBlendColorChannelList;
    panel->flags = 1;
    options = MenuWidget_CreateNode(0x31, parent, parent);
    options->draw = Menu_DrawBlendColorOptionList;
    panel->linkedNext = options;
    options->linkedPrevious = panel;
    MenuWidget_SetCurrentNode(panel->cursor_x < 0 ? options : panel);
    g_MenuBlendColorBackup = Draw_GetBlendColor();
}

/* Up/down arrows over the three colour channels; the pair over the selected
 * channel lights up while that direction is held. */
void Menu_DrawBlendColorArrows(MenuWidgetNode *panel) {
    int dimmed0;
    int dimmed1;
    int dimmed2;
    int dimmed3;
    int dimmed4;
    int dimmed5;
    u16 directionHeld;
    s32 selectedChannel;

    selectedChannel = MenuWidget_GridCellIndex(MenuWidget_GetChild(panel, 0));
    directionHeld = MenuInput_GetStatusFlags() & 0x1000;
    Draw_OffsetCursor(0x12, 5);
    dimmed0 = 0;
    if ((selectedChannel != 0) || (directionHeld == 0)) {
        dimmed0 = 1;
    }
    Draw_SetTextDimmed(dimmed0);
    Draw_AllocSprite(0x4A);
    Draw_OffsetCursor(0x20, 0);
    dimmed1 = 0;
    if ((selectedChannel != 1) || (directionHeld == 0)) {
        dimmed1 = 1;
    }
    Draw_SetTextDimmed(dimmed1);
    Draw_AllocSprite(0x4A);
    Draw_OffsetCursor(0x20, 0);
    dimmed2 = 0;
    if ((selectedChannel != 2) || (directionHeld == 0)) {
        dimmed2 = 1;
    }
    Draw_SetTextDimmed(dimmed2);
    Draw_AllocSprite(0x4A);
    directionHeld = MenuInput_GetStatusFlags() & 0x4000;
    Draw_OffsetCursor(-0x40, 0x19);
    dimmed3 = 0;
    if ((selectedChannel != 0) || (directionHeld == 0)) {
        dimmed3 = 1;
    }
    Draw_SetTextDimmed(dimmed3);
    Draw_AllocSprite(0x4B);
    Draw_OffsetCursor(0x20, 0);
    dimmed4 = 0;
    if ((selectedChannel != 1) || (directionHeld == 0)) {
        dimmed4 = 1;
    }
    Draw_SetTextDimmed(dimmed4);
    Draw_AllocSprite(0x4B);
    Draw_OffsetCursor(0x20, 0);
    dimmed5 = 0;
    if ((selectedChannel != 2) || (directionHeld == 0)) {
        dimmed5 = 1;
    }
    Draw_SetTextDimmed(dimmed5);
    Draw_AllocSprite(0x4B);
}

s32 Menu_StepColorSelect(MenuWidgetNode *panel, s32 flags) {
    s32 shift;
    s32 choice;
    s32 color;
    s32 level;
    s32 value;
    s32 others;
    MenuWidgetNode *options;

    shift = MenuWidget_GridCellIndex(MenuWidget_GetChild(panel, 0)) << 3;
    color = Draw_GetBlendColor();
    if (flags & 0x1000) {
        others = color & ~(0xFF << shift);
        level = ((color >> shift) & 0xFF) + 2;
        if (level < 0xE9) {
            value = level << shift;
        } else {
            value = 0xE8 << shift;
        }
        Draw_BlendColor(others | value);
        Menu_PlayMoveSound();
    } else if (flags & 0x4000) {
        others = color & ~(0xFF << shift);
        level = ((color >> shift) & 0xFF) - 2;
        if (level >= 0x20) {
            value = level << shift;
        } else {
            value = 0x20 << shift;
        }
        Draw_BlendColor(others | value);
        Menu_PlayMoveSound();
    } else if (flags & 0x10000) {
        options = MenuWidget_GetChild(panel, 1);
        choice = MenuWidget_GridCellIndex(options);
        if (choice == 0) {
            g_SavedDrawBlendColor[0] = Draw_GetBlendColor();
            MenuWidget_DestroyNode(panel);
        } else if (choice == 1) {
            Draw_BlendColor(0x404040);
            g_SavedDrawBlendColor[0] = Draw_GetBlendColor();
            MenuWidget_DestroyNode(panel);
        } else {
            options->cursor_x = 0;
            options->cursor_y = 0;
            MenuWidget_SetCurrentNode(options);
            MenuWidget_GetChild(panel, 0)->cursor_x = -1;
        }
        Menu_PlayConfirmSound();
    } else if (flags & 0x40) {
        MenuWidget_DestroyNode(panel);
        Draw_BlendColor(g_MenuBlendColorBackup);
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_DrawBlendColorChannelList(int node) {
    MenuWidget_DrawList((MenuWidgetNode *)node, Menu_DrawBlendColorChannelRow);
}

int g_MenuScreenBaseYBackup;

void Menu_DrawBlendColorOptionList(int node) {
    MenuWidget_DrawList((MenuWidgetNode *)node, Menu_DrawBlendColorOptionRow);
}

void Menu_OpenScreenAdjustView(MenuWidgetNode *parent) {
    MenuWidgetNode *node;

    node = MenuWidget_CreateSimpleNode(0x38, parent, 0, 1);
    node->draw = Menu_DrawScreenAdjustPanel;
    node->update = Menu_ScreenAdjustInputHandler;
    MenuWidget_SetCurrentNode(node);
    g_MenuScreenBaseYBackup = Draw_GetBaseY();
}

void Menu_DrawScreenAdjustPanel(void) {
    Draw_OffsetCursor(0x10, 0xA);
    Draw_PrintSignedNumberWidth3(8 - Draw_GetBaseY());
    Draw_SetTextDimmed(VSync(-1) & 0x10);
    Draw_OffsetCursor(-0x10, -0x69);
    Draw_AllocSprite(0x7B);
    Draw_OffsetCursor(0, 0xBE);
    Draw_EmitGlyph(0x7B, 2);
}

int Menu_ScreenAdjustInputHandler(void *node, int input) {
    int zero_arg;

    {
        if ((input & 0x1000) != 0) {
            zero_arg = 0;
            input = -1;
        } else if ((input & 0x4000) != 0) {
            zero_arg = 0;
            input = 1;
        } else {
            goto screen_adjust_other;
        }
        Draw_SetBaseOffsetPosition(zero_arg, input);
        Menu_PlayMoveSound();
        return 1;
    }
screen_adjust_other:

    if ((input & 0x10000) != 0) {
        MenuWidget_DestroyNode(node);
        Menu_PlayConfirmSound();
        return 1;
    }

    if ((input & 0x40) != 0) {
        Draw_SetBaseOffsetPosition(0, g_MenuScreenBaseYBackup - Draw_GetBaseY());
        MenuWidget_DestroyNode(node);
        Menu_PlayCancelSound();
    }

    return 1;
}

void Menu_CreateExpReviewView(void);
void Menu_DrawExpReviewPanel(void);
s32 Menu_ConfirmExpAllocation(MenuWidgetNode *node, s32 flags);
void Menu_CreateLevelUpBanner(void);
void Menu_DrawLevelUpBanner(void);
void Menu_CreateLevelUpResultPanel(void);
void Menu_DrawBonusPointAnimFrame(void);
s32 Menu_StepParasiteScreen(MenuWidgetNode *node, s32 flags);

int g_BonusPointBarAnimProgress, g_BonusPointBarAnimStep;
int g_BonusPointDisplayValue, g_MenuBonusPointDisplayTarget;
int g_MenuBonusPointBarAnimActive;
int g_MenuExpAllocTarget;
int g_MenuExpReviewLevel;
int g_MenuHpMaxAnimTimer;
int g_MenuHpMaxDisplayTarget, g_MenuHpMaxDisplayValue;
int g_MenuLevelDisplayTarget, g_MenuLevelDisplayValue;
int g_MenuLevelUpAnimTimer;
/* The stat loop sign-extends the allocation levels (lh in retail). */

void Aya_SetTotalExp(s32 exp_delta, s32 pe_bonus_delta, void *wayne_items) {
    AyaSaveState *save;
    s32 previous;
    s16 *stat_src;
    s32 i;
    s32 stat_level;

    Menu_SetBattleEquipMode(0);
    BattleCmd_SyncActiveAmmo();

    save = &g_AyaSaveState;
    /* Match debt: retail reads the total and clears the bonus pool through
     * the save base held in s2; a known constant folds to absolute loads. */
    PE1_COMPILER_LAUNDER(save);
    previous = save->total_exp;
    g_MenuPendingTotalExp = previous;
    g_MenuExpAllocTarget = previous + exp_delta;
    g_MenuExpReviewLevel = Aya_StatBinarySearch(g_MenuPendingTotalExp, Aya_GetLevelExpTable());
    g_MenuSelectionLocked = 1;

    g_AyaBonusPoolRecord.pool = g_AyaBonusPoolRecord.pool + pe_bonus_delta;
    g_MenuLevelDisplayValue = g_AyaSaveState.level;
    g_MenuHpMaxDisplayValue = g_AyaSaveState.max_hp;
    g_MenuBonusPointDisplayTarget = g_AyaSaveState.bonus_points;
    g_BonusPointDisplayValue = g_AyaSaveState.bonus_points;
    g_MenuLevelDisplayTarget = Aya_StatBinarySearch(g_MenuExpAllocTarget, Aya_GetLevelExpTable());

    if (g_MenuLevelDisplayValue < g_MenuLevelDisplayTarget) {
        s32 total;
        s32 bonus = g_MenuBonusPointDisplayTarget + g_AyaBonusPoolRecord.pool;
        if (bonus <= 0x1869F) {
            total = bonus;
        } else {
            total = 0x1869F;
        }
        g_MenuBonusPointDisplayTarget = total;
        save->pe_bonus_pool = 0;
    }

    stat_src = g_AyaSaveState.stats.levels;
    i = 0;
    do {
        g_BonusPointStatDeltas[i] = *stat_src++;
        Aya_StatQueryLevelAndSubLevel(i, g_BonusPointStatDeltas[i], &g_BonusPointStatQueryResults[i], 0);
        g_BonusPointStatMultipliers[i] = (g_MenuLevelDisplayTarget - g_MenuLevelDisplayValue) * 10;
        i++;
    } while (i < 7);

    Aya_StatQueryLevelAndSubLevel(0, g_BonusPointStatDeltas[0] + g_BonusPointStatMultipliers[0], &stat_level, 0);
    g_MenuHpMaxDisplayTarget = Battle_GetScaledMaxHP(stat_level);
    g_BonusPointBarAnimStep = 2;
    Inv_LoadWayneItemsAsOverride(wayne_items);
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
    node->appearance.gradientPoints = g_MenuExpReviewPanelPoints;
}

void Menu_DrawExpReviewPanel(void) {
    s32 can_step;
    s32 level;
    s32 exp_to_next;

    can_step = g_MenuPendingTotalExp < g_MenuExpAllocTarget;
    if (!can_step) {
        Akao_FlushBgmVolumeFade();
    }

    level = Aya_StatBinarySearch(g_MenuPendingTotalExp, Aya_GetLevelExpTable());
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
    Draw_PrintRawText(Text_LookupTable4(9));
    Draw_OffsetCursor(0x50, 0);
    Draw_PrintNumberWidth6(
        0xF423F < g_MenuPendingTotalExp ? 0xF423F : g_MenuPendingTotalExp);
    Draw_OffsetCursor(2, 2);
    Draw_AllocSprite(0x8A);
    Draw_StatePop();

    Draw_OffsetCursor(0, 0xE);
    Draw_StatePush();
    Draw_PrintRawText(Text_LookupTable4(0xA));
    Draw_OffsetCursor(0x50, 0);
    Draw_PrintNumberWidth6(exp_to_next);
    Draw_OffsetCursor(2, 2);
    Draw_AllocSprite(0x8A);
    Draw_StatePop();

    Draw_OffsetCursor(0, 0x14);
    Draw_StatePush();
    Draw_PrintRawText(Text_LookupTable4(0xC));
    Draw_OffsetCursor(0x74, 0);
    Draw_PrintNumberWidth2(Menu_GetBattleCount());
    Draw_OffsetCursor(2, 0);
    Draw_PrintRawText(Text_LookupTable4(0xD));
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
            new_level = Aya_StatBinarySearch(g_MenuPendingTotalExp, Aya_GetLevelExpTable());
            if (new_level != Aya_StatBinarySearch(g_AyaSaveState.total_exp, Aya_GetLevelExpTable())) {
                Menu_CreateLevelUpResultPanel();
                Akao_SetBgmVolumeFade();
            } else if (Menu_GetBattleCount() != 0) {
                Menu_StepSaveLoadScreen();
            } else {
                Inv_SetActiveList(0xA, 0);
            }
            g_AyaSaveState.total_exp = g_MenuPendingTotalExp;
        }
        Menu_PlayConfirmSound();
    }
    return 1;
}

void Menu_CreateLevelUpBanner(void) {
    MenuWidget_CreateSimpleNode(0x16, 0, 0, 0)->draw = Menu_DrawLevelUpBanner;
}

void Menu_DrawLevelUpBanner(void) {
    Draw_OffsetCursor(0, 4);
    Draw_PrintCenteredText(Text_LookupTable4(0x19));
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

    g_MenuPendingTotalExp = g_AyaSaveState.total_exp;
    g_MenuExpAllocTarget = g_AyaSaveState.total_exp;
    g_MenuLevelDisplayValue = g_AyaSaveState.level;
    g_MenuExpReviewLevel = g_AyaSaveState.level;
    g_MenuLevelDisplayTarget = g_AyaSaveState.level;
    g_MenuHpMaxDisplayValue = g_AyaSaveState.max_hp;
    g_MenuHpMaxDisplayTarget = g_AyaSaveState.max_hp;
    g_BonusPointDisplayValue = g_AyaSaveState.bonus_points;

    if (gained_points < 0) {
        gained_points = 0;
    }
    g_MenuBonusPointDisplayTarget = g_AyaSaveState.bonus_points + gained_points;

    source = g_AyaSaveState.stats.levels;
    stat = 0;
    multipliers = g_BonusPointStatMultipliers;
    query_results = g_BonusPointStatQueryResults;
    deltas = g_BonusPointStatDeltas;
    do {
        value = *source++;
        *deltas = value;
        Aya_StatQueryLevelAndSubLevel(stat, value, query_results, 0);
        *multipliers = 0;
        multipliers++;
        query_results++;
        deltas++;
        stat++;
    } while (stat < 7);

    Aya_StatQueryLevelAndSubLevel(0, g_BonusPointStatDeltas[0], scratch, 0);
    g_BonusPointBarAnimStep = 2;
    Inv_LoadWayneItemsAsOverride(0);
    Menu_CreateLevelUpResultPanel();
    MenuWidget_OffsetPosition(MenuWidget_FindByModeAndSelectedBase(1, 0x15), 0, 0x20);
    if (gained_points > 0) {
        Akao_SetBgmVolumeFade();
    }
    Menu_ResetInputState();
}

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
    root->appearance.gradientPoints = g_MenuLevelUpPanelPoints;
    g_MenuBonusPointBarAnimActive = 1;
    Menu_InitStateTables();
}

void Menu_InitStateTables(void) {
    int i;

    for (i = 0; i < 8; i++) {
        g_MenuStatBarSettleTimers[i] = 0;
        g_MenuStatBarWidths[i] = 0;
    }
}

void Menu_DrawBonusPointAnimFrame(void) {
    int var_a0;
    int var_a0_2;
    int var_a0_3;
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
    if (g_AyaSaveState.level < g_MenuLevelDisplayValue) {
        var_a0 = 0x8080;
    }
    Draw_SetColor(var_a0);
    Draw_OffsetCursor(0xF, 0x10);
    Draw_PrintNumberWidth5(g_MenuLevelDisplayValue + 1);
    Draw_SetColor(0x808080);
    Draw_OffsetCursor(-0x3C, 0x14);
    Draw_AllocSprite(0x99);
    if (g_MenuHpMaxDisplayValue < g_MenuHpMaxDisplayTarget) {
        temp_v1 = g_MenuHpMaxAnimTimer;
        g_MenuHpMaxAnimTimer = temp_v1 + 1;
        var_s0 = 1;
        if (temp_v1 >= 0) {
            g_MenuHpMaxAnimTimer = 0;
            g_MenuHpMaxDisplayValue += 1;
        }
    }
    var_a0_2 = 0x808080;
    if (g_AyaSaveState.max_hp < g_MenuHpMaxDisplayValue) {
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
    if (g_AyaSaveState.bonus_points < temp_v1) {
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
    s32 temp_v1;
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
            statDestination = g_AyaSaveState.stats.transfer;
            var_a2 = 0;
            statMultipliers = g_BonusPointStatMultipliers;
            statDeltas = g_BonusPointStatDeltas;
            do {
                *statDestination++ = *statDeltas++ + *statMultipliers++;
                var_a2 += 1;
            } while (var_a2 < 9);
            temp_v0 = g_MenuLevelDisplayValue;
            temp_v1 = g_MenuHpMaxDisplayValue;
            value = g_BonusPointDisplayValue;
            g_AyaSaveState.level = temp_v0;
            g_AyaSaveState.max_hp = temp_v1;
            g_AyaSaveState.bonus_points = value;
            Inv_RecalcSlotStats();
            MenuWidget_DestroyNode(arg0);
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x1E));
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

#define NULL ((void *)0)

s32 Menu_OpenEquipSlotSwap(s32 arg0) {
    s32 temp_s2;
    s32 temp_v1;
    MenuWidgetNode *temp_a0;
    MenuWidgetNode *temp_s1;
    MenuWidgetNode *temp_v0;

    temp_s2 = MenuWidget_FindByModeAndSelectedBase(1, 1) != NULL;
    if (temp_s2 == 0) {
        MenuInput_InitQueue();
        MenuWidget_CreateSimpleNode(0x1B, 0, 0, 0)->draw = Menu_DrawAmmoTypeHeader;
        temp_v0 = MenuWidget_CreateSimpleNode(1, 0, 0, 0);
        temp_s1 = MenuWidget_CreateNode(1, temp_v0, temp_v0);
        temp_a0 = temp_s1;
        temp_v0->update = Menu_InventoryInputHandler;
        temp_s1->draw = Menu_DrawSelectableEquipSlotList;
        MenuWidget_SetCurrentNode(temp_a0);
        temp_s1->itemAction = (MenuWidgetItemAction)Inv_SwapSlots;
        temp_s1->refreshItems = Menu_RebuildSelectableMask;
        Inv_RebuildSelectableMask();
        g_InvSwapTargetIndex = -1;
        g_InvSelectedItemIndex = -1;
        Draw_SetPrimCallback(temp_s1, Inv_GetAyaSlotLimit());
        g_MenuActionSubmenuOpen = 0;
        if (g_MenuEquipSwapSource != 0) {
            temp_v1 = g_MenuEquipSwapSource - 1;
            temp_s1->cursor_x = (temp_v1 & 1);
            temp_s1->cursor_y = ((temp_v1 >> 1) & 0x7F);
            temp_s1->scroll_y = (temp_v1 >> 8);
        }
        Menu_CreateContextHelpPanel();
        MenuWidget_FindByModeAndSelectedBase(2, 1)->itemAction = NULL;
        Inv_SortSlotsByPriority();
        g_MenuActionSubmenuOpen = 1;
        g_AyaFullInvHold = arg0;
        g_MenuSelectionLocked = 0;
        Menu_SetBattleEquipMode(0);
    }
    return temp_s2;
}
