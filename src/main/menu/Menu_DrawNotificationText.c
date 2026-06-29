extern char g_MenuTwoLineDialogText[];

void Draw_OffsetCursor(int arg0, int arg1);
void Draw_PrintCenteredText(void *arg0);

void Menu_DrawNotificationText(int *arg0) {
    char *text;

    Draw_OffsetCursor(0, 0xA);
    text = g_MenuTwoLineDialogText;
    if (arg0[9] == 0x3D) {
        text += 0x40;
    }
    Draw_PrintCenteredText(text);
}
