/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/menu_dialog.h"
#include "pe1/menu_inventory.h"

int g_MemCardProgressPrimList;

void MemCard_DrawProgress(void);
int Menu_MemCardProgressInputHandler(void);

void Menu_CreateMemCardProgressWidget(int arg0) {
    void *node;
    void *widget;

    node = MenuWidget_GetCurrentNode();
    widget = MenuWidget_CreateSimpleNode(0x27, node, 0, 1);
    *(void **)((char *)widget + 0x30) = MemCard_DrawProgress;
    *(void **)((char *)widget + 0x2C) = Menu_MemCardProgressInputHandler;
    MenuWidget_SetCurrentNode(widget);
    g_MemCardProgressPrimList = arg0;
}
