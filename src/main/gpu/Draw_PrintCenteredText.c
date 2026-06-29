/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_DrawTextBoxWidth;

void Draw_PrintCenteredTextInWidth(int arg0, int arg1);

void Draw_PrintCenteredText(int arg0) {
    Draw_PrintCenteredTextInWidth(arg0, g_DrawTextBoxWidth);
}
