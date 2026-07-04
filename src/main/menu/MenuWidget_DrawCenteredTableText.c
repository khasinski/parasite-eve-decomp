/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_DrawTextPosX;

int Str_LookupTable4(void);
void Draw_PrintCenteredTextInWidth(int arg0, int arg1);

void MenuWidget_DrawCenteredTableText(void) {
    Draw_PrintCenteredTextInWidth(Str_LookupTable4(), g_DrawTextPosX);
}
