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
