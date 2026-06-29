/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

void *g_MenuSelectedItemData;

void Draw_OffsetCursor(int arg0, int arg1);
void Sfx_CursorRenderData(void *arg0);

void Menu_DrawEquipItemIcon(void) {
    Draw_OffsetCursor(6, 6);
    Sfx_CursorRenderData(g_MenuSelectedItemData);
}
