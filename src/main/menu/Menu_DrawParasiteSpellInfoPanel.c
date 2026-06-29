/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int D_8009CFF4;

void Draw_OffsetCursor(int arg0, int arg1);
int Str_LookupTable4(int arg0);
int Draw_PrintCenteredText(int arg0);
int Str_LookupTable8(int arg0);
int Str_LookupTableC(int arg0);
int Draw_PrintRawText(int arg0);

void Menu_DrawParasiteSpellInfoPanel(void) {
    Draw_OffsetCursor(2, 4);
    Draw_PrintCenteredText(Str_LookupTable4(0x26));
    Draw_OffsetCursor(0, 0x10);
    Draw_PrintCenteredText(Str_LookupTable8(D_8009CFF4 + 0xEB));
    Draw_OffsetCursor(2, 0x14);
    Draw_PrintRawText(Str_LookupTableC(D_8009CFF4 + 0xEB));
}
