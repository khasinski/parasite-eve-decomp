/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuActiveWidget;
int g_MenuEquipMode;
extern unsigned char D_800923B8[][8];

int MenuWidget_GridCellIndex(int);
void Draw_OffsetCursor(int, int);
void Draw_SetTextDimmed(int);
void Draw_AllocSprite(int);

void Menu_DrawEquipSlotListItem(int arg0) {
    if (arg0 != MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
        Draw_SetTextDimmed(1);
    }

    Draw_OffsetCursor(-2, -2);

    if (g_MenuEquipMode != 0) {
        Draw_AllocSprite(D_800923B8[g_MenuEquipMode][arg0]);
    }
}
