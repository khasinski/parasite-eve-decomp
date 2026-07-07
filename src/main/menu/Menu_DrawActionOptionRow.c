/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuActionTextBase;

void Draw_SetTextDimmed(int);
void MenuWidget_DrawCenteredTableText(int);

void Menu_DrawActionOptionRow(int arg0) {
    Draw_SetTextDimmed(0);
    MenuWidget_DrawCenteredTableText(g_MenuActionTextBase + arg0);
}
