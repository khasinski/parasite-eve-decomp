/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

extern int D_8009CEF4;

int MenuWidget_GridCellIndex(int arg0);
void Draw_SetTextDimmed(int enable);
void Draw_OffsetCursor(int x, int y);
void Draw_AllocSprite(int sprite);

void func_80050FB8(int index) {
    if (index != MenuWidget_GridCellIndex(D_8009CEF4)) {
        Draw_SetTextDimmed(1);
    }

    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(index + 0x66);
}
