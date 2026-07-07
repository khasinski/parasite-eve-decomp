/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

int g_MenuEquipMode;

int g_MenuActiveWidget;

void Menu_DrawEquipSlotListItem(int arg0);
void MenuWidget_DrawList(int arg0, void (*callback)(int));
void Draw_SetTextDimmed(int value);
void Draw_AllocSprite(int arg0);
void Draw_OffsetCursor(int x, int y);

int Menu_GetEquipMode(void) {
    return g_MenuEquipMode;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/menu20", Menu_OpenInventoryScreen);

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
