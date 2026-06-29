extern char g_MenuTwoLineDialogText[];

void Draw_OffsetCursor(int arg0, int arg1);
void Draw_PrintCenteredText(void *arg0);

void Menu_DrawTwoLineDialogText(void) {
    Draw_OffsetCursor(0, 0xA);
    Draw_PrintCenteredText(g_MenuTwoLineDialogText);
    Draw_OffsetCursor(0, 0xE);
    Draw_PrintCenteredText(g_MenuTwoLineDialogText + 0x40);
}
