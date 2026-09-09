void Inv_ShowItemDescriptionPanel(void);

void Draw_OffsetCursor(int, int);
void Draw_AllocSprite(int);

void MenuWidget_DrawCenteredTableText(int arg0);

void Menu_DrawArmorEquipItem(void) {
    Inv_ShowItemDescriptionPanel();
}

void Menu_DrawMemCardPortItem(int arg0) {
    Draw_OffsetCursor(-2, -2);
    if (arg0 < 2) {
        Draw_AllocSprite(arg0 + 0x84);
    } else {
        Draw_AllocSprite(0x62);
    }
}

void Menu_DrawMainMenuItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x28);
}

void MenuWidget_DrawCenteredTableText(int arg0);
void Draw_DropShadow(void);
int Menu_GetActionSubmenuSelection(void);

void Menu_DrawActionSubmenuItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x2E);

    if (Menu_GetActionSubmenuSelection() == arg0) {
        Draw_DropShadow();
    }
}

void MenuWidget_DrawCenteredTableText(int arg0);
void Draw_DropShadow(void);
int MenuWidget_GetColumnLayoutMode(void);

void Menu_DrawBlendOptionItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x31);

    if (MenuWidget_GetColumnLayoutMode() == arg0) {
        Draw_DropShadow();
    }
}

void MenuWidget_DrawCenteredTableText(int arg0);

void Menu_DrawNotificationItem(void) {
    MenuWidget_DrawCenteredTableText(8);
}

void Menu_DrawInvPanelItemStub(void) {
}

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_BonusPointBarAnimProgress;
extern int g_BonusPointStatDeltas[];
extern int g_BonusPointStatMultipliers[];

void Stat_QueryLevelAndSubLevel(int, int, int *, int);
void Draw_OffsetCursor(int x, int y);
void Draw_AllocSprite(int);
void Draw_PrintNumberWidth2(int arg0);

void Menu_DrawStatAllocationItem(int arg0) {
    int value;
    int local;

    arg0 += 1;
    value = g_BonusPointStatDeltas[arg0] + ((g_BonusPointBarAnimProgress * g_BonusPointStatMultipliers[arg0]) >> 7);

    Draw_OffsetCursor(6, 0);
    Draw_AllocSprite(arg0 + 0x8C);
    Draw_OffsetCursor(0x4A, 0);
    Stat_QueryLevelAndSubLevel(arg0, value, &local, 0);
    Draw_PrintNumberWidth2(local + 1);
}


extern int g_StatBaseTable[];

void Draw_OffsetCursor(int, int);
void Draw_StatePush(void);
void Draw_StatePop(void);
void Draw_PrintTextById(int);
void Draw_PrintNumberWidth2Unk(int);
void Draw_PrintNumberWidth3Unk(int);

void Menu_DrawScreenModeItem(int arg0) {
    Draw_StatePush();
    Draw_PrintTextById((arg0 & 1) | 0x6A);

    if ((arg0 >= 2) || (g_StatBaseTable[arg0] >= 100)) {
        Draw_OffsetCursor(0x28, 3);
        Draw_PrintNumberWidth3Unk(g_StatBaseTable[arg0]);
    } else {
        Draw_OffsetCursor(0x2D, 3);
        Draw_PrintNumberWidth2Unk(g_StatBaseTable[arg0]);
    }

    Draw_StatePop();
}

extern int D_800A18EC[];

void Menu_UpdateStatBarAnimation(int);
void Stat_QueryLevelAndSubLevel(int, int, int *, int);
void Draw_OffsetCursor(int, int);
void Draw_AllocSprite(int);
void Draw_PrintNumberWidth2(int);

void Menu_DrawParasiteAbilityItem(int arg0) {
    int value;
    int local;

    value = D_800A18EC[arg0];

    Draw_OffsetCursor(2, 1);
    Draw_AllocSprite(arg0 + 0x91);
    Draw_OffsetCursor(0x42, -1);

    arg0 += 5;
    Stat_QueryLevelAndSubLevel(arg0, value, &local, 0);
    Draw_PrintNumberWidth2(local + 1);
    Draw_OffsetCursor(2, 0);
    Menu_UpdateStatBarAnimation(arg0);
    Draw_OffsetCursor(-0x44, 0xE);
}

/* MASPSX_FLAGS: -G8 --use-comm-section */

unsigned char *g_MenuItemIconGlyphTable;

void Draw_OffsetCursor(int x, int y);
void Draw_StatePush(void);
void Draw_StatePop(void);
void Draw_AllocTexturedQuad(int);

void Menu_DrawItemIcon(int arg0) {
    Draw_StatePush();
    Draw_OffsetCursor(2, 0);
    Draw_AllocTexturedQuad(g_MenuItemIconGlyphTable[arg0]);
    Draw_StatePop();
}
