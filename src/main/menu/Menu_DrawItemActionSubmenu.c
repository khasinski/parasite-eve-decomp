/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned char u8;

int g_InvItemUsableFlag;
int g_MenuSelectedItemList;
int g_MenuSelectedItemSlot;
int g_MenuSelectedItemIndex;

int Inv_RestoreSelection(int arg0);
void Draw_OffsetCursor(int arg0, int arg1);
void Draw_StatePush(void);
u8 *Inv_LookupActiveListData(int arg0);
void Draw_AllocSprite(int arg0);
void Draw_PrintTextById(int arg0);
int Str_LookupTable4(int arg0);
int Draw_MeasureTextWidth(int arg0);
void Draw_StatePop(void);
void Sfx_DrawActiveListSlot(int arg0);

void Menu_DrawItemActionSubmenu(void) {
    int selected;
    int icon;
    u8 *entry;

    selected = Inv_RestoreSelection(g_MenuSelectedItemList);
    Draw_OffsetCursor(4, 4);
    Draw_StatePush();

    if (g_MenuSelectedItemSlot >= 0) {
        entry = Inv_LookupActiveListData(selected);
        entry += g_MenuSelectedItemSlot;
        icon = entry[0x15] & 0x1F;
        if (g_InvItemUsableFlag != 0) {
            Draw_AllocSprite(icon + 0x22);
        } else {
            Draw_AllocSprite(icon + 0x36);
        }
        Draw_OffsetCursor(0x24, 0);
    } else {
        Draw_PrintTextById(0x66);
        Draw_OffsetCursor(Draw_MeasureTextWidth(Str_LookupTable4(0x66)), 0);
    }

    Draw_PrintTextById((g_MenuSelectedItemIndex & 1) | 0x64);
    Draw_StatePop();

    if ((g_MenuSelectedItemIndex & 1) == 0) {
        Draw_OffsetCursor(0, 0xE);
        Sfx_DrawActiveListSlot(selected);
        Draw_OffsetCursor(0, 0xE);
        Draw_PrintTextById(0x67);
    }

    Draw_OffsetCursor(0, 0x14);
    Draw_PrintTextById(0x68);
}
