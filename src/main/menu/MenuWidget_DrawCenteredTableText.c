/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

extern int D_8009D164;

int Str_LookupTable4(int text_id);
void Draw_PrintCenteredTextInWidth(int text, int width);

void MenuWidget_DrawCenteredTableText(int text_id) {
    Draw_PrintCenteredTextInWidth(Str_LookupTable4(text_id), D_8009D164);
}
