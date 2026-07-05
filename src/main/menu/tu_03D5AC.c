#include "include_asm.h"

void MenuWidget_NavScrollTo(int arg0);

extern char g_MenuTwoLineDialogText[];

void Draw_OffsetCursor(int arg0, int arg1);
void Draw_PrintCenteredText(void *arg0);

void Menu_CloseNotificationDialogs(void) {
    MenuWidget_NavScrollTo(0x28);
    MenuWidget_NavScrollTo(0x3D);
}

void Menu_DrawNotificationText(int *arg0) {
    char *text;

    Draw_OffsetCursor(0, 0xA);
    text = g_MenuTwoLineDialogText;
    if (arg0[9] == 0x3D) {
        text += 0x40;
    }
    Draw_PrintCenteredText(text);
}

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/tu_03D5AC", Menu_CreateTwoLineDialog);

void Menu_DrawTwoLineDialogText(void) {
    Draw_OffsetCursor(0, 0xA);
    Draw_PrintCenteredText(g_MenuTwoLineDialogText);
    Draw_OffsetCursor(0, 0xE);
    Draw_PrintCenteredText(g_MenuTwoLineDialogText + 0x40);
}
