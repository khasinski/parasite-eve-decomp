/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef int s32;

void Akao_FlushBgmVolumeFade(void);
void *Aya_GetLevelExpTable(void);
s32 Stat_BinarySearch(s32 value, void *table);
void *MenuWidget_FindByModeAndSelectedBase(s32 mode, s32 selected_base);
void MenuWidget_DestroyNode(void *node);
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

extern s32 g_MenuPendingTotalExp;
extern s32 g_MenuExpAllocTarget;
extern s32 D_8009CFF0;
extern s32 D_8009CF78;

void Menu_DrawExpReviewPanel(void) {
    s32 can_step;
    s32 level;
    s32 exp_to_next;
    s32 shown_exp;

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
    if (D_8009CFF0 < level) {
        D_8009CFF0 = level;
        D_8009CF78 = 0x3C;
        if (MenuWidget_FindByModeAndSelectedBase(1, 0x16) == 0) {
            Menu_CreateLevelUpBanner();
        }
    }

    if (D_8009CF78 != 0) {
        D_8009CF78--;
        if (D_8009CF78 == 0) {
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x16));
        }
    }

    Draw_OffsetCursor(4, 4);
    Draw_StatePush();
    Draw_PrintRawText(Str_LookupTable4(9));
    Draw_OffsetCursor(0x50, 0);
    shown_exp = g_MenuPendingTotalExp;
    if (shown_exp > 0xF423F) {
        shown_exp = 0xF423F;
    }
    Draw_PrintNumberWidth6(shown_exp);
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
