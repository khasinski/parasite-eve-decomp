/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

unsigned char *g_MenuSelectedItemData;

void Draw_OffsetCursor(int arg0, int arg1);
int Str_LookupTableC(int arg0);
void Draw_PrintRawText(int arg0);

void Menu_DrawEquipItemName(void) {
    Draw_OffsetCursor(4, 4);
    Draw_PrintRawText(Str_LookupTableC(g_MenuSelectedItemData[4] - 1));
}
