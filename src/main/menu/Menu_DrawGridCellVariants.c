/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/menu_inventory.h"

extern int D_8009CEF4;

void Draw_SetTextDimmed(int enable);
void Draw_OffsetCursor(int x, int y);
void Draw_AllocSprite(int sprite);

void func_80050F64(int index) {
    if (index != MenuWidget_GridCellIndex(D_8009CEF4)) {
        Draw_SetTextDimmed(1);
    }
    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(index + 0x63);
}

void func_80050FB8(int index) {
    if (index != MenuWidget_GridCellIndex(D_8009CEF4)) {
        Draw_SetTextDimmed(1);
    }
    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(index + 0x66);
}

void func_8005100C(int index) {
    if (index != MenuWidget_GridCellIndex(D_8009CEF4)) {
        Draw_SetTextDimmed(1);
    }
    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(index + 0x62);
}
