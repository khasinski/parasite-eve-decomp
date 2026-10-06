/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

int D_8009CFF4;

void *MenuWidget_FindByModeAndSelectedBase(int arg0, int arg1);
void *MenuWidget_CreateSimpleNode(int arg0, int arg1, int arg2, int arg3);
void Menu_DrawParasiteSpellInfoPanel(void);

void Draw_OffsetCursor(int arg0, int arg1);
int Str_LookupTable4(int arg0);
int Draw_PrintCenteredText(int arg0);
int Str_LookupTable8(int arg0);
int Str_LookupTableC(int arg0);
int Draw_PrintRawText(int arg0);

void Menu_CreateParasiteSpellInfoPanel(int arg0) {
    void *widget;

    if (MenuWidget_FindByModeAndSelectedBase(1, 0x1E) == 0) {
        widget = MenuWidget_CreateSimpleNode(0x1E, 0, 0, 0);
        *(void **)((char *)widget + 0x30) = Menu_DrawParasiteSpellInfoPanel;
        MenuWidget_CreateSimpleNode(0x1C, 0, 0, 0);
    }

    D_8009CFF4 = arg0;
}

void Menu_DrawParasiteSpellInfoPanel(void) {
    Draw_OffsetCursor(2, 4);
    Draw_PrintCenteredText(Str_LookupTable4(0x26));
    Draw_OffsetCursor(0, 0x10);
    Draw_PrintCenteredText(Str_LookupTable8(D_8009CFF4 + 0xEB));
    Draw_OffsetCursor(2, 0x14);
    Draw_PrintRawText(Str_LookupTableC(D_8009CFF4 + 0xEB));
}
