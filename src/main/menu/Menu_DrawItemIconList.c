/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern unsigned char *g_MenuItemIconGlyphTable;

void Menu_DrawItemIcon(int arg0);
void MenuWidget_DrawList(int arg0, void (*callback)(int));
void *MenuWidget_FindByModeAndSelectedBase(int arg0, int arg1);
unsigned char *Str_LookupTable4(int arg0);

void Menu_DrawItemIconList(int arg0) {
    void *entry;
    int index;

    entry = MenuWidget_FindByModeAndSelectedBase(2, 0x17);
    if (entry != 0) {
        index = *(int *)((char *)entry + 0x48) + 0x73;
    } else {
        index = 0x75;
    }

    g_MenuItemIconGlyphTable = Str_LookupTable4(index);
    MenuWidget_DrawList(arg0, Menu_DrawItemIcon);
}
