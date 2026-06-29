/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int D_8009CFC0;

void Draw_OffsetCursor(int arg0, int arg1);
void Draw_AllocSprite(int arg0);
void Draw_PrintTextById(int arg0);

void Menu_DrawItemListHeader(void) {
    Draw_OffsetCursor(4, 4);
    Draw_AllocSprite(D_8009CFC0);
    Draw_OffsetCursor(0x20, 0);
    Draw_PrintTextById(0x1C);
    Draw_OffsetCursor(-0x20, 0x10);
    Draw_PrintTextById(0x1D);
}
