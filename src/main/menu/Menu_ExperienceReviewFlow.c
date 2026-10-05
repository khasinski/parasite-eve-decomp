/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/aya.h"
#include "pe1/psyq_nop.h"

void Menu_SetBattleEquipMode(s32 mode);
void BattleCmd_SyncActiveAmmo(void);
void *Aya_GetLevelExpTable(void);
s32 Stat_BinarySearch(s32 value, void *table);

s32 Battle_GetScaledMaxHP(s32 level);
void Inv_LoadWayneItemsAsOverride(void *items);
void Menu_CreateExpReviewView(void);
void Akao_SetBgmVolumeFade(void);
void Menu_ResetInputState(void);



/* Distinct views preserve the original loads and store around the bonus update. */
extern u16 D1E_read[16] __asm__("D_800C0E1E");
extern u16 D1E_write[16] __asm__("D_800C0E1E");
extern u16 D1E_bonus[16] __asm__("D_800C0E1E");
/* This loop sign-extends the stat allocations (lh in the original). */
extern s16 D_800C0E28_signed[7] __asm__("D_800C0E28");
extern struct { char bytes[32]; } stat_out_alias __asm__("D_800A18B4");
extern s32 D_800A18D8[];
extern s32 D_800A18FC[];
extern s32 D_8009CFE8;
extern s32 D_8009CFEC;
extern s32 D_8009CFF0;
extern s32 D_8009CEFC;
extern s32 D_8009CF60;
extern s32 D_8009CF64;
extern s32 D_8009CF68;
extern s32 D_8009CF6C;
extern s32 D_8009CF70;
extern s32 D_8009CF74;
extern s32 D_8009CF84;

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
    D_8009CFE8 = previous_exp;
    asm volatile("" : : : "memory");
    D_8009CFEC = previous_exp + exp_delta_reg;
    D_8009CFF0 = Stat_BinarySearch(D_8009CFE8, Aya_GetLevelExpTable());
    D_8009CEFC = 1;

    D1E_write[0] = D1E_read[0] + pe_bonus_delta_reg;
    D_8009CF60 = aya_level_view[0];
    D_8009CF64 = aya_max_hp_view[0];
    D_8009CF74 = aya_bonus_view[0];
    D_8009CF68 = aya_bonus_view[0];
    D_8009CF6C = Stat_BinarySearch(D_8009CFEC, Aya_GetLevelExpTable());

    if (D_8009CF60 < D_8009CF6C) {
        s32 cap = 0x1869F;
        register s32 result asm("$5") = 0x1869F;
        s32 bonus = D_8009CF74 + D1E_bonus[0];
        if (bonus <= cap) result = bonus;
        D_8009CF74 = result;
        save->pe_bonus_pool = 0;
    }

    stat_src = D_800C0E28_signed;
    i = 0;
    do {
        D_800A18D8[i] = *stat_src++;
        Stat_QueryLevelAndSubLevel(i, D_800A18D8[i], (s32 *)((u8 *)&stat_out_alias + i * 4), 0);
        D_800A18FC[i] = (D_8009CF6C - D_8009CF60) * 10;
        i++;
    } while (i < 7);

    Stat_QueryLevelAndSubLevel(0, D_800A18D8[0] + D_800A18FC[0], &stat_level, 0);
    D_8009CF70 = Battle_GetScaledMaxHP(stat_level);
    D_8009CF84 = 2;
    Inv_LoadWayneItemsAsOverride(wayne_items_reg);
    Menu_CreateExpReviewView();
    Akao_SetBgmVolumeFade();
    Menu_ResetInputState();
}
#include "pe1/menu_widget.h"

MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, int arg1, int arg2, int arg3);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
void Menu_DrawExpReviewPanel(void);
s32 Menu_ConfirmExpAllocation(s32 unused, s32 flags);
extern int D_800922F4[];

void Menu_CreateExpReviewView(void) {
    MenuWidgetNode *node;

    node = MenuWidget_CreateSimpleNode(0x14, 0, 0, 0);
    node->draw = Menu_DrawExpReviewPanel;
    node->update = Menu_ConfirmExpAllocation;
    MenuWidget_SetCurrentNode(node);
    node->target_x = (int)&D_800922F4[0];
}
#include "common.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

void Akao_FlushBgmVolumeFade(void);
void *Aya_GetLevelExpTable(void);
s32 Stat_BinarySearch(s32 value, void *table);
MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(s32 mode, s32 selected_base);
void MenuWidget_DestroyNode();
void Menu_CreateLevelUpBanner(void);
void Draw_OffsetCursor(s32 x, s32 y);
void Draw_StatePush(void);
void Draw_StatePop(void);
void *Str_LookupTable4(s32 index);
void Draw_PrintRawText(void *text);
void Draw_PrintNumberWidth6(s32 value);
void Draw_PrintNumberWidth2(s32 value);
void Draw_AllocSprite(s32 sprite_id);
s32 Menu_GetBattleCount(void);

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

#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "include_asm.h"

void MenuWidget_DestroyNode();
void Menu_StepSaveLoadScreen(void);
void Menu_CreateLevelUpResultPanel(void);
void Inv_SetActiveList(s32 arg0, s32 arg1);
void Menu_PlayConfirmSound(void);
void Akao_SetBgmVolumeFade(void);
void Akao_FlushBgmVolumeFade(void);
s32 Menu_GetBattleCount(void);
s32 Stat_BinarySearch(s32 value, void *table);
void *Aya_GetLevelExpTable(void);

extern s32 g_MenuPendingTotalExp;
extern s32 g_MenuExpAllocTarget;
extern s32 g_AyaSaveTotalExp[];
#define g_AyaSaveTotalExp (g_AyaSaveTotalExp[0])

s32 Menu_ConfirmExpAllocation(s32 unused, s32 flags) {
    s32 confirm;
    s32 new_level;

    confirm = flags & 0x10000;
    if (confirm != 0) {
        if (g_MenuPendingTotalExp < g_MenuExpAllocTarget) {
            g_MenuPendingTotalExp = g_MenuExpAllocTarget;
        } else {
            MenuWidget_DestroyNode();
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
