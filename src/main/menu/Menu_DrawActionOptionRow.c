/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuActionTextBase;

void Draw_SetTextDimmed(int);
void MenuWidget_DrawCenteredTableText(int);

asm(".globl func_800509A8");
asm("func_800509A8 = Menu_DrawActionOptionRow");

void Menu_DrawActionOptionRow(int arg0) {
    Draw_SetTextDimmed(0);
    MenuWidget_DrawCenteredTableText(g_MenuActionTextBase + arg0);
}
