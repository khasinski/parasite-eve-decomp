/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/aya.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_runtime.h"
#include "pe1/draw_level_bar.h"
#include "pe1/draw_state.h"
#include "pe1/draw_wipe_rect.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/memcard.h"
#include "pe1/menu_background.h"
#include "pe1/menu_bonus_stats.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_inventory_root.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_buffers.h"
#include "pe1/resident_tables.h"
#include "pe1/save.h"
#include "pe1/stat_modifiers.h"
#include "pe1/text.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_BonusPointDisplayValue;
int g_BonusPointBarAnimStep;
int g_BonusPointBarAnimProgress;
int g_InvItemUsableFlag;
MenuWidgetNode *g_MenuActiveWidget;
int g_MenuBattleStatusOverlayActive;
int g_MenuEquipMode;
int g_MenuItemUseMode;
int g_MenuItemRenameMode;
int g_MenuItemDiscardMode;
int g_InvSelectedItemIndex;
int g_MenuSelectionLocked;
int g_MenuLayoutLocked;
int g_MenuSaveBgFadeIndex;
int g_MenuSaveBgFadeLutLen;
int g_MenuSaveBgFadeTint;

/* Save-screen background fade: gamma curve, per-frame darkening of the
 * captured frame and the fade state machine setters. Contiguous at
 * 0x80042C78 and joined by the fade state words. MemCard_InitSlotState
 * (historical name) arms the fade-in. */

int g_MenuSaveBgFadeState;
int g_MenuSaveBgFadeStep;
int g_MenuSaveBgFadeHeight;

void Menu_SaveBgInitFade(void) {
    g_MenuSaveBgFadeState = 0;
    g_MenuSaveBgFadeLutLen = 0;
    g_MenuSaveBgFadeStep = 0;
    g_MenuSaveBgFadeHeight = 0x20;
    Menu_ComputeGammaLut(0x90, 0xFF);
    g_MenuSaveBgFadeTint = 0x48;
}

void Menu_SaveBgSetFadeTarget(int value) {
    g_MenuSaveBgFadeTint = value;
}

void Menu_ComputeGammaLut(int arg0, int arg1) {
    u8 *base;
    u8 *ptr;
    int scale;
    int limit;
    u8 *iter_end;
    u8 *check_end;

    base = g_MenuSaveBgFadeLut;
    base[0] = 0;
    ptr = base;
    limit = 0x100;
    scale = limit - arg0;
    check_end = ptr + 0xF;
    arg0 <<= 8;
    if (ptr < check_end) {
        iter_end = check_end;
        do {
            if (*ptr >= arg1) {
                base = g_MenuSaveBgFadeLut;
                limit = (ptr - base) + 1;
                g_MenuSaveBgFadeLutLen = limit;
                return;
            }
            ptr[1] = (arg0 + (scale * *ptr)) >> 8;
            ptr++;
        } while (ptr < iter_end);
    }

out:
    base = g_MenuSaveBgFadeLut;
    limit = (ptr - base) + 1;
    g_MenuSaveBgFadeLutLen = limit;
}

void Menu_SaveBgApplyFadeStep(void) {
    int pos;
    int level;
    int bias;
    int attenuation;
    u16 *src;
    u16 *dst;
    int i;

    pos = g_MenuSaveBgFadeIndex + g_MenuSaveBgFadeStep;
    g_MenuSaveBgFadeIndex = pos;
    if (pos < 0) {
        g_MenuSaveBgFadeIndex = 0;
        g_MenuSaveBgFadeStep = 0;
        g_MenuSaveBgFadeState = 0;
    } else if (pos >= g_MenuSaveBgFadeLutLen) {
        g_MenuSaveBgFadeIndex = g_MenuSaveBgFadeLutLen - 1;
        g_MenuSaveBgFadeStep = 0;
        g_MenuSaveBgFadeState = 7;
    }

    level = g_MenuSaveBgFadeLut[g_MenuSaveBgFadeIndex];
    {
        int product = level * g_MenuSaveBgFadeTint;
        bias = product << 5;
    }
    attenuation = level * (g_MenuSaveBgFadeTint + 0x100);
    src = dst = g_GameState.save_background_source;
    dst = g_GameState.save_background_destination;

    for (i = 0; i < (g_MenuSaveBgFadeHeight << 8); i++) {
        int pixel = *src++;
        int color;

        if (pixel != 0) {
            int r = pixel & 0x1F;
            int g = (pixel >> 5) & 0x1F;
            int b = (pixel >> 10) & 0x1F;
            color = pixel & 0x8000;

            {
                int mixed = r << 16;
                mixed = bias + mixed - attenuation * r;
                r = mixed >> 16;
            }
            g = (bias + (g << 16) - (attenuation * g)) >> 16;
            b = (bias + (b << 16) - (attenuation * b)) >> 16;
            color |= r;
            *dst = color | (g << 5) | (b << 10);
        } else {
            *dst = 0;
        }
        dst++;
    }

    {
        unsigned shade = (unsigned)bias >> 13;
        g_RenderDrawEnvArray[1].b0 = shade;
        g_RenderDrawEnvArray[1].g0 = shade;
        g_RenderDrawEnvArray[1].r0 = shade;
        g_RenderDrawEnvArray[0].b0 = shade;
        g_RenderDrawEnvArray[0].g0 = shade;
        g_RenderDrawEnvArray[0].r0 = shade;
    }
}

int Menu_SaveBgIsFadeActive(void) {
    return g_MenuSaveBgFadeState != 0;
}

void MemCard_InitSlotState(void) {
    int value = g_GeomOtZ[0];

    g_MenuSaveBgFadeState = 1;
    g_MenuSaveBgFadeStep = 1;
    g_MenuSaveBgFadeIndex = 0;
    g_MenuSaveBgFadeHeight = value;

    /* value is a zero-extended byte, so the < 0 arm is dead at runtime,
     * but it is what retail compiled (bgez). */
    if (value < 0) {
        g_MenuSaveBgFadeHeight = 1;
    } else if (value >= 0x21) {
        g_MenuSaveBgFadeHeight = 0x20;
    }
}

void Menu_SaveBgStartFadeOut(void) {
    g_MenuSaveBgFadeState = 5;
    g_MenuSaveBgFadeStep = -1;
}

void Menu_SaveBgClearFade(void) {
    g_MenuSaveBgFadeState = 0;
}

void Menu_SaveBgAdvanceFade(void) {
    RECT rect;
    int next;

    switch (g_MenuSaveBgFadeState) {
    case 0:
        return;

    case 1:
        next = 2;
        break;

    case 2:
        next = 3;
        break;

    case 3:
        rect.y = 0x1E0;
        rect.x = 0;
        rect.w = 0x100;
        rect.h = g_MenuSaveBgFadeHeight;
        StoreImage(&rect, (void *)g_RenderFrontBufferBase[0]);
        next = 4;
        break;

    case 5:
        g_MenuSaveBgFadeState = 6;
        Menu_SaveBgApplyFadeStep();
        return;

    case 4:
    case 6:
        next = 5;
        break;

    case 7:
        return;

    default:
        return;
    }

    g_MenuSaveBgFadeState = next;
}

void Menu_DrawSaveBg(void) {
    RECT rect;

    if (g_MenuSaveBgFadeState == 6) {
        rect.y = 0x1E0;
        rect.h = g_MenuSaveBgFadeHeight;
        rect.x = 0;
        rect.w = 0x100;
        LoadImage(&rect, (u_long *)g_RenderBackBufferBase[0]);
    }
}

int g_MenuSaveBgFadeState;

int Menu_SaveBgIsFadeComplete(void) {
    return g_MenuSaveBgFadeState == 7;
}

void Inv_FetchSelectedInventoryItem(void) {
    int node;

    node = MenuWidget_FindByModeAndSelectedBase(2, 7);
    if (node == 0) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 0xD);
        if (node == 0) {
            node = MenuWidget_FindByModeAndSelectedBase(2, 0x10);
        }
    }

    Inv_GetPackedListItem(MenuWidget_GridCellIndex(node));
}

void Menu_HandleInput(int arg0) {
    register int value;
    int flag;
    int blocked;

    if (arg0 >= Inv_GetPackedListCount()) {
        return;
    }

    if (g_MenuItemRenameMode != 0 || g_MenuItemDiscardMode != 0) {
        if (Inv_TestSelectionBit(arg0) == 0) {
            Draw_SetTextDimmed(1);
        }
    }

    value = Inv_GetPackedListItem(arg0);
    blocked = Inv_IsAyaInventorySlotReserved(value);

    if (blocked != 0) {
        if (g_MenuActiveWidget->selected_base != 0xD) {
            if (g_MenuLayoutLocked == 0 && g_MenuItemUseMode == 0) {
                Draw_SetTextDimmed(1);
            }
        }
    }

    if (g_MenuLayoutLocked != 0) {
    flag = 0;
    if (g_StatBaseTable[0] + g_StatBaseTable[1] + g_StatBaseTable[2] + g_StatBaseTable[3] != 0) {
        if ((Inv_LookupActiveListData(value)->flags & 0x40) == 0) {
            int mask;

            if (Inv_GetActiveListItemType(value) >= 6) {
                mask = 1 << Inv_GetActiveListItemType(value);
            } else {
                mask = 0x3E;
            }

            if (Inv_SetupSlotDisplay(mask) != 0) {
                flag = 1;
            }
        }
    }

    if (flag == 0) {
        Draw_SetTextDimmed(1);
    }
    }

    Menu_DrawActiveListSlot(value);
    if (blocked != 0) {
        Draw_DropShadow();
    }
}

int g_MenuItemSelectFlag;

void Menu_SetItemContext(int arg0) {
    g_MenuItemSelectFlag = arg0;
}

int Menu_CheckItemAffordable(int actionId)
{
    int total, remaining, enabled, allowed, category;
    int action = actionId;
    int bits;
    ItemDataRecord *item;
    remaining = BattleCmd_GetRemainingAmmo(&total);
    item = Item_LookupBaseData(action + 235);
    bits = item->flags >> Menu_GetBattleEquipMode();
    if (action == 5) {
        if (remaining >= total / 3) enabled = bits & 1;
        else enabled = 0;
    } else if (action != 6) {
        if (action != 19) {
            if (remaining >= Inv_GetSlotHighlightState(action, 0)) enabled = bits & 1;
            else enabled = 0;
        } else {
            if (remaining >= total) enabled = bits & 1;
            else enabled = 0;
        }
    } else {
        if (remaining > 0) enabled = bits & 1;
        else enabled = 0;
    }
    /* These actions are unavailable while the global restriction is active. */
    allowed = 0;
    if (!g_MenuItemSelectFlag || ((unsigned)(action - 6) >= 3 && action != 10 && action != 19)) {
        allowed = 1;
    }
    enabled &= allowed;
    if (action == 5 && Battle_GetEnemyEscapeFlag()) enabled = 0;
    if (action == 6) {
        ItemDataRecord *weapon;
        weapon = Inv_LookupActiveListData(g_InvTrackedSlots[0]);
        category = weapon->kind;
        if (category == 8) enabled = 0;
        else {
            if (category && (unsigned)category < 8) {
                category -= 4;
                if (category <= 0) category = 1;
            } else {
                category = weapon->kind < 19 ? 0 : weapon->kind - 18;
            }
            if (category == 1 || category == 3) {
                int ammo = Inv_GetWeaponCategoryAmmoBase(category - 1);
                enabled &= weapon->ammo + ammo > 0 ? 1 : 0;
            } else enabled = 0;
        }
    }
    if (action < 3 || action == 18) {
        if (g_AyaHpCurrent[0] >= g_AyaHpMax[0]) enabled = 0;
    }
    return enabled;
}

int Save_ClassifyPlaytime(int playTime) {
    int result;

    if (playTime >= 0x80) {
        if (playTime >= 0x138) {
            if (playTime >= 0x1B0) {
                if (playTime >= 0x218) {
                    if (playTime < 0x2D3) {
                        result = 5;
                    } else {
                        result = 6;
                    }
                } else {
                    result = 4;
                }
            } else {
                result = 3;
            }
        } else {
            result = 2;
        }
    } else {
        result = 1;
    }

    return result;
}

#define NULL ((void *)0)

void Menu_DrawSaveSlotEntry(s32 arg0) {
    register MemCardSaveSlot *slot;
    s32 index;
    int dx;
    int text_id;
    int dy;
    u8 state;

    index = arg0;
    slot = MemCard_GetSlot(g_MenuActiveWidget->selected_base - 0x25, index);
    if (slot != NULL) {
        state = slot->state;
        switch (state) {
        case MEMCARD_SLOT_OCCUPIED:
            if (index == MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
                Menu_SetSaveSlotBlendBase(slot->blendColor);
            } else {
                Draw_OffsetCursor(-2, -2);
                Draw_BlendColor(slot->blendColor);
                Draw_EmitWipeBarRect(g_MenuActiveWidget->draw_state, g_MenuActiveWidget->disabled, 1);
                Draw_BlendColor(g_SavedDrawBlendColor[0]);
                Draw_OffsetCursor(2, 2);
            }
            Draw_OffsetCursor(2, 2);
            if (Save_GetMetadataWindowIndex() == 0) {
                Draw_PrintRawText(slot->primaryTitle.text);
            } else {
                Draw_PrintRawText(slot->alternateTitle.text);
            }
            Draw_OffsetCursor(0x54, 0);
            if (slot->exGameIndex != 0) {
                Draw_AllocSprite(0x95);
                Draw_OffsetCursor(0x1A, 2);
                Draw_AllocSprite(0x96);
                Draw_OffsetCursor(0x22, 1);
                Draw_PrintNumberWidth2Unk(slot->exGameIndex + 1);
                dx = 4;
                dy = -3;
            } else {
                dx = 0x4A;
                dy = 0;
            }
            Draw_OffsetCursor(dx, dy);
            Draw_PrintTextById(0x5A);
            Draw_OffsetCursor(0x19, 0);
            Draw_PrintNumberWidth2(index + 1);
            Draw_OffsetCursor(-0xC0, 0xF);
            Draw_AllocSprite(0x53);
            Draw_OffsetCursor(0x1E, 1);
            Draw_PrintNumberWidth3Unk(slot->levelIndex + 1);
            Draw_OffsetCursor(8, -1);
            Draw_AllocSprite(0x54);
            Draw_OffsetCursor(0x10, 1);
            Draw_PrintNumberWidth4Unk(slot->currentHp);
            Draw_AllocSprite(0x4C);
            Draw_OffsetCursor(5, 0);
            Draw_PrintNumberWidth4Unk(slot->maxHp);
            Draw_OffsetCursor(8, -1);
            Draw_AllocSprite(0x55);
            Draw_OffsetCursor(0x1A, 1);
            Draw_PrintTimeValue(slot->playTimeMinutes, 0);
            Draw_OffsetCursor(-0xC0, 0xC);
            if (slot->titleStyleFlag != 0) {
                Draw_PrintTextById(0x60);
                return;
            }
            Draw_PrintTextById(0xB);
            Draw_OffsetCursor(0x1E, 0);
            Draw_PrintNumberWidth2(slot->progressStage);
            Draw_OffsetCursor(0x14, 0);
            Draw_PrintRawText(Text_LookupMapLocationText(slot->mapNumber));
            return;
        case MEMCARD_SLOT_EMPTY:
            Draw_OffsetCursor(0, 0x12);
            text_id = 0x41;
            MenuWidget_DrawCenteredTableText(text_id);
            Draw_OffsetCursor(0x5C, -0x10);
            break;
        }
    }
}

int g_MenuItemContextFlag;

void Menu_UpdateStatBarAnimation(s32 arg0) {
    s32 sp10;
    s32 sp14;
    s32 current;
    s32 timer;

    Aya_StatQueryDistanceAndSubLevel(
        arg0,
        g_BonusPointStatDeltas[arg0] + ((g_BonusPointBarAnimProgress * g_BonusPointStatMultipliers[arg0]) >> 7),
        &sp10,
        &sp14);

    current = g_MenuStatBarWidths[arg0];
    if (sp14 >= current) {
        g_MenuStatBarWidths[arg0] = (current + sp14) >> 1;
    } else if (current < 0x30) {
        g_MenuStatBarWidths[arg0] = 0x30;
        g_MenuStatBarSettleTimers[arg0] = 4;
    } else {
        timer = g_MenuStatBarSettleTimers[arg0];
        if (timer > 0) {
            timer -= 2;
        }
        g_MenuStatBarSettleTimers[arg0] = timer;
        if (timer == 0) {
            g_MenuStatBarWidths[arg0] = sp14;
        }
    }

    Draw_AllocTexturedRectAlt(sp10, g_MenuStatBarWidths[arg0]);
}

int Menu_ClampRange(int arg0) {
    arg0 &= 0x1FF;
    g_MenuItemContextFlag = arg0;
    if (arg0 == 0) {
        g_MenuItemContextFlag = 0x1FF;
    }
    return 0;
}

int Menu_GetItemContextFlag(void) {
    return g_MenuItemContextFlag;
}

/* Inventory stat-panel setup and its adjacent draw callbacks. */

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
        temp_v0->update = Menu_StepItemSelectScreen;
        temp_s1->draw = Menu_DrawItemList;
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

void Menu_DrawStatusPanel(void);
void Menu_DrawStatsList(void);

void Menu_CreateBonusPointAllocationView(void) {
    int *var_s3;
    int *var_s0;
    int *var_s2;
    s32 var_s1;
    u16 *var_s4;
    MenuWidgetNode *temp_v0;
    MenuWidgetNode *temp_v0_2;
    MenuWidgetNode *temp_v1_reg;

    temp_v0 = temp_v1_reg = MenuWidget_CreateSimpleNode(0x12, 0, 0, 0);
    temp_v1_reg->draw = Menu_DrawStatusPanel;
    temp_v1_reg->appearance.gradientPoints = g_MenuStatusPanelPoints;
    (MenuWidget_CreateSimpleNode(0x18, 0, 0, 0))->draw =
        Menu_DrawBonusPointSlotValue;
    temp_v0_2 = temp_v1_reg = MenuWidget_CreateSimpleNode(0x2D, 0, 0, 0);
    var_s4 = &g_AyaSaveState.stats.levels[0];
    var_s1 = 0;
    var_s0 = g_BonusPointStatDeltas;
    var_s3 = g_BonusPointStatQueryResults;
    var_s2 = g_BonusPointStatMultipliers;
    temp_v1_reg->draw = Menu_DrawStatsList;
    temp_v1_reg->appearance.gradientPoints = g_MenuStatsListPanelPoints;
    for (; var_s1 < 7; ++var_s1) {
        *var_s0 = *var_s4++;
        *var_s2 = 0;
        Aya_StatQueryLevelAndSubLevel(var_s1, *var_s0, var_s3, 0);
        var_s3++;
        var_s0++;
        var_s2++;
    }
    g_BonusPointDisplayValue = g_AyaSaveState.bonus_points;
}

void Menu_DrawStatusPanel(void) {
    s32 var_a0;

    Draw_OffsetCursor(2, 2);
    Draw_AllocSprite(0x47);
    Draw_OffsetCursor(0x28, 2);
    Draw_PrintRawText(Save_GetActiveMetadataBuffer());
    Draw_OffsetCursor(0, 0x15);
    Draw_AllocSprite(0x97);
    Draw_OffsetCursor(0x3C, 0);
    Draw_PrintNumberWidth3(g_AyaSaveState.level + 1);
    Draw_OffsetCursor(-0x78, 0x1A);
    Draw_AllocSprite(0x98);
    Draw_OffsetCursor(0x42, 0);
    if (g_AyaSaveState.level < 0x62U) {
        var_a0 = ((s32 *)Aya_GetLevelExpTable())[g_AyaSaveState.level + 1] - g_AyaSaveState.total_exp;
    } else {
        var_a0 = 0;
    }
    Draw_PrintNumberWidth6(var_a0);
    Draw_OffsetCursor(-0x7C, 0x15);
    Draw_AllocSprite(0x94);
    Draw_OffsetCursor(0, 0x10);
    Menu_DrawActiveListSlot(g_AyaSaveState.equipped_weapon_slot);
    Draw_OffsetCursor(0, 0x10);
    if (g_AyaSaveState.equipped_armor_slot >= 0) {
        Menu_DrawActiveListSlot(g_AyaSaveState.equipped_armor_slot);
        return;
    }
    Draw_PrintTextById(0x39);
}

void Menu_DrawStatsList(void) {
    s32 sp10;
    int var_a0;
    int var_a1;
    s32 *var_s3;
    s32 *var_s4;
    s32 temp_s0;
    s32 temp_s1;
    s32 var_a0_2;
    s32 var_s2;

    var_s4 = Battle_GetModifierTable() + 1;
    Draw_OffsetCursor(4, 5);
    var_s2 = 1;
    var_s3 = &g_BonusPointStatDeltas[1];
    do {
        temp_s1 = *var_s4;
        temp_s0 = *var_s3 + ((g_BonusPointBarAnimProgress * g_BonusPointStatMultipliers[var_s2]) >> 7);
        Draw_OffsetCursor(2, 0);
        Draw_AllocSprite(var_s2 + 0x8C);
        Draw_OffsetCursor(0x4A, 0);
        Aya_StatQueryLevelAndSubLevel(var_s2, temp_s0, &sp10, 0);
        var_s4 += 1;
        Draw_PrintNumberWidth2(sp10 + 1);
        if (temp_s1 != 0) {
            Draw_OffsetCursor(6, 0);
            var_a0 = 0x70;
            if (temp_s1 > 0) {
                var_a0_2 = 0x6F;
                var_a0 = var_a0_2;
            }
            Draw_PrintTextById(var_a0);
            /* The printed marker ID is dead; reuse its scratch for the sign. */
            var_a0 = temp_s1 < 0;
            var_a0_2 = temp_s1;
            if (var_a0) {
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

#define CLOSE_SELECT_VIEWS() { \
    MenuWidget_NavScrollTo(0x2d); \
    MenuWidget_NavScrollTo(0x18); \
    MenuWidget_NavScrollTo(0x12); \
}

/* Matching debt: the three inventory-root arguments retain retail registers.
 * Empty barriers preserve the return paths and call delay slots. */
int Menu_StepItemSelectScreen(MenuWidgetNode *node, unsigned int buttons)
{
    int index = 0;
    int mask;
    int action;
    MenuWidgetNode *child;
    int confirm = buttons & 0x10000;
    if (confirm) {
        child = MenuWidget_GetChild(node, 0);
        index = MenuWidget_GridCellIndex(child);
        if (Menu_GetBattleEquipMode()) {
            mask = g_MenuItemContextFlag & 0x1f;
        } else {
            mask = g_MenuItemContextFlag & 0x1ef;
        }
        action = -1;
        goto check;
        while (1) {
            if (index < 0) break;
            index -= mask & 1;
            mask >>= 1;
            ++action;
        check:
            if (action >= 9) break;
        }
        switch (action) {
        case 0:
            CLOSE_SELECT_VIEWS();
            Menu_CreateEquipScreen(child);
            break;
        case 1:
            CLOSE_SELECT_VIEWS();
            Menu_CreateSkillActionScreen(child);
            break;
        case 2:
            CLOSE_SELECT_VIEWS();
            g_InvItemUsableFlag = 1;
            Menu_CreateInventoryListView(child);
            Menu_CreateEquipItemSelectionView(child);
            Menu_StepSkillList(MenuWidget_FindByModeAndSelectedBase(2, 5), 0);
            break;
        case 3:
            CLOSE_SELECT_VIEWS();
            g_InvItemUsableFlag = 0;
            Menu_CreateInventoryListView(child);
            Menu_CreateEquipItemSelectionView(child);
            /* An int-returning call keeps cross-jumping from merging this
             * case into case 2, which retail keeps separate. */
            ((int (*)(MenuWidgetNode *, int))Menu_StepSkillList)(MenuWidget_FindByModeAndSelectedBase(2, 5), 0);
            break;
        case 4:
            if (Battle_IsActiveWrapped()) {
                CLOSE_SELECT_VIEWS();
                Inv_SetActiveList(8, 0);
            } else {
                Menu_PlayErrorSound();
                index = 1;
                asm volatile("" : "=r"(index) : "0"(index));
                return index;
            }
            break;
        case 5:
            CLOSE_SELECT_VIEWS();
            Menu_CreateMainMenuView(child);
            break;
        case 6:
            CLOSE_SELECT_VIEWS();
            Menu_CreateInvSwapView(child, 0);
            break;
        case 7:
            CLOSE_SELECT_VIEWS();
            g_MenuLayoutLocked = 1;
            Inv_SelectActiveList(0);
            g_StatBaseTable[0] = Inv_CountByCategory(0xe) ? 999 : Inv_CountByCategory(0xc);
            g_StatBaseTable[1] = Inv_CountByCategory(0xf) ? 999 : Inv_CountByCategory(0xd);
            if (g_MenuEquipMode) {
                g_StatBaseTable[2] = Inv_CountWayneStorageItemType(0xe) ? 999 : Inv_CountWayneStorageItemType(0xc);
                g_StatBaseTable[3] = Inv_CountWayneStorageItemType(0xf) ? 999 : Inv_CountWayneStorageItemType(0xd);
            } else {
                g_StatBaseTable[3] = 0;
                g_StatBaseTable[2] = 0;
            }
            Menu_StepInventoryRoot(0x33e, -1, -1);
            g_MenuBattleStatusOverlayActive = 0;
            break;
        case 8:
            CLOSE_SELECT_VIEWS();
            g_MenuItemUseMode = 1;
            Menu_StepInventoryRoot(0x37e, -1, -1);
            g_MenuBattleStatusOverlayActive = 0;
            break;
        default:
            goto return_one;
        }
        Menu_PlayConfirmSound();
        index = 1;
        asm volatile("" : "=r"(index) : "0"(index));
        return index;
    }
    if (buttons & 0x40) {
        MenuWidget_InitPool();
        Inv_SetActiveList(9, 0);
        if (!Menu_GetBattleEquipMode()) BattleCmd_ResetTableCursor();
        Menu_PlayCancelSound();
    return_one:
        index = 1;
        asm volatile("" : "=r"(index) : "0"(index));
        return index;
    }
    {
        int down = buttons & 2;
        asm volatile("" : "=r"(index) : "0"(index));
        if (down) Menu_InitScrollDirection();
    }
    return index;
}
