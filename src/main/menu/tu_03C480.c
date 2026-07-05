void *MenuWidget_CreateSimpleNode(int arg0, int arg1, int arg2, int arg3);
void Menu_DrawLevelUpBanner(void);

void Draw_OffsetCursor(int arg0, int arg1);
int Str_LookupTable4(int arg0);
void Draw_PrintCenteredText(int arg0);

void Menu_CreateLevelUpBanner(void) {
    *(void **)((char *)MenuWidget_CreateSimpleNode(0x16, 0, 0, 0) + 0x30) = Menu_DrawLevelUpBanner;
}

void Menu_DrawLevelUpBanner(void) {
    Draw_OffsetCursor(0, 4);
    Draw_PrintCenteredText(Str_LookupTable4(0x19));
}
