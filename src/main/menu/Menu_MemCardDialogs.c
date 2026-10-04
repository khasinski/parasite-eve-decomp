#include "common.h"
#include "pe1/menu_dialog.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)

void MemCard_InitState(void);
void MemCard_SetDialogActive(s32 active);
void Queue_Init(void);

extern s32 g_McDialogMode;
void Menu_DrawContextHelpText(MenuWidgetNode *panel);
s32 Menu_MemCardPortSelectHandler(void *node, s32 flags);
int Menu_IsMemCardSlotSelectable(int arg0);
void Menu_DrawMemCardPortList(int arg0);

void Menu_OpenMemCardSelectDialog(s32 arg0) {
    MenuWidgetNode *temp_a0;
    MenuWidgetNode *temp_v0;

    temp_v0 = MenuWidget_CreateSimpleNode(0x24, MenuWidget_GetCurrentNode(), 0, 0);
    temp_a0 = MenuWidget_CreateNode(0x24, temp_v0, temp_v0);
    temp_v0->update = (void (*)(void))Menu_MemCardPortSelectHandler;
    temp_a0->draw = (void (*)(void))Menu_DrawMemCardPortList;
    temp_a0->selectionAvailable = Menu_IsMemCardSlotSelectable;
    if (temp_a0->cursor_y == 2) {
        temp_a0->cursor_y = 0;
    }
    MenuWidget_SetCurrentNode(temp_a0);
    if (MenuWidget_FindByModeAndSelectedBase(1, 0x13) == NULL) {
        MenuWidget_CreateSimpleNode(0x13, 0, 0, 0)->draw = (void (*)(void))Menu_DrawContextHelpText;
    }
    MenuWidget_FindByModeAndSelectedBase(1, 0x13)->visible_rows = 0x24;
    g_McDialogMode = arg0;
    if (arg0 != 0) {
        MemCard_SetDialogActive(1);
        MemCard_InitState();
    }
    Queue_Init();
}

void Menu_OpenStartupMemCardDialog(void) {
    s32 one;
    void *callback_2c;
    void *callback_30;
    void *callback_8c;
    MenuWidgetNode *child;
    MenuWidgetNode *parent;

    parent = MenuWidget_CreateSimpleNode(0x24, MenuWidget_GetCurrentNode(), 0, 0);
    child = MenuWidget_CreateNode(0x24, parent, parent);
    callback_2c = Menu_MemCardPortSelectHandler;
    callback_30 = Menu_DrawMemCardPortList;
    parent->update = (void (*)(void))callback_2c;
    child->draw = (void (*)(void))callback_30;
    callback_8c = Menu_IsMemCardSlotSelectable;
    child->selectionAvailable = (int (*)(int))callback_8c;
    one = 1;
    if (child->cursor_y == 2) {
        child->cursor_y = 0;
    }
    MenuWidget_SetCurrentNode(child);
    if (MenuWidget_FindByModeAndSelectedBase(1, 0x13) == NULL) {
        MenuWidget_CreateSimpleNode(0x13, 0, 0, 0)->draw = (void (*)(void))Menu_DrawContextHelpText;
    }
    MenuWidget_FindByModeAndSelectedBase(1, 0x13)->visible_rows = 0x24;
    g_McDialogMode = one;
    MemCard_SetDialogActive(1);
    MemCard_InitState();
    Queue_Init();
}
