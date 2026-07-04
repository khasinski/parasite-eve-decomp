/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_DrawTextPosX;

void Draw_PrintCenteredTextInWidth(int arg0, int arg1);

void MenuWidget_DrawCenteredText(int arg0) {
    Draw_PrintCenteredTextInWidth(arg0, g_DrawTextPosX);
}
