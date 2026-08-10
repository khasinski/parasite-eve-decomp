/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "pe1/memcard.h"

int g_MenuActiveWidget;

void Menu_DrawMemCardPortItem(int arg0);
void MenuWidget_DrawList(int arg0, void (*callback)(void));
void Draw_SetTextDimmed(int value);
void Draw_AllocSprite(int arg0);
void Draw_OffsetCursor(int x, int y);

int g_McDialogMode;
int g_MenuActiveWidget;

MemCardSaveSlot *MemCard_GetSlot(unsigned int port, int slot);
int Save_GetTitleStyleFlag(void);

void Menu_DrawMemCardPortList(int arg0) {
    int count;

    g_MenuActiveWidget = arg0;
    MenuWidget_DrawList(arg0, Menu_DrawMemCardPortItem);
    Draw_SetTextDimmed(1);
    count = *(int *)(arg0 + 0x38);
    while (count != 0) {
        Draw_AllocSprite(0x68);
        Draw_OffsetCursor(0, 0x10);
        count--;
    }
}

int Menu_IsMemCardSlotCompatible(int arg0) {
    MemCardSaveSlot *ptr;
    int result;

    if (g_McDialogMode != 0) {
        ptr = MemCard_GetSlot(*(int *)(g_MenuActiveWidget + 0x24) - 0x25, arg0);
        result = ptr->state != MEMCARD_SLOT_UNAVAILABLE;
    } else {
        ptr = MemCard_GetSlot(*(int *)(g_MenuActiveWidget + 0x24) - 0x25, arg0);
        if (ptr->state != MEMCARD_SLOT_OCCUPIED) {
            result = 0;
            goto done;
        }

        result = ptr->titleStyleFlag;
        result ^= Save_GetTitleStyleFlag();
        result = (unsigned int)result < 1;
    }

done:
    return result;
}
