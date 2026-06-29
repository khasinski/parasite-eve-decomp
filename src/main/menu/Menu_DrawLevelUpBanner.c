void Draw_OffsetCursor(int arg0, int arg1);
int Str_LookupTable4(int arg0);
void Draw_PrintCenteredText(int arg0);

void Menu_DrawLevelUpBanner(void) {
    Draw_OffsetCursor(0, 4);
    Draw_PrintCenteredText(Str_LookupTable4(0x19));
}
