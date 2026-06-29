/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuActiveWidget;

void Menu_DrawEquipSlotListItem(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));
void Draw_SetTextDimmed(int arg0);
void Draw_AllocSprite(int arg0);
void Draw_OffsetCursor(int arg0, int arg1);

void Menu_DrawEquipListContainer(int arg0) {
    int count;

    g_MenuActiveWidget = arg0;
    MenuWidget_DrawList(arg0, Menu_DrawEquipSlotListItem);
    Draw_SetTextDimmed(1);
    count = *(int *)(arg0 + 0x38);
    while (count != 0) {
        Draw_AllocSprite(0x68);
        Draw_OffsetCursor(0, 0x10);
        count--;
    }
}
