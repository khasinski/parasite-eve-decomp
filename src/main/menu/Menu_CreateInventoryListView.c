#include "common.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_inventory.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */


#define NULL ((void *)0)

void MenuWidget_SetColumnLayout(void *node, int value);
void MenuWidget_ClearColumnLayout(void *node);

extern s32 g_InvItemUsableFlag;
extern s32 g_MenuLayoutLocked;
int Menu_InventoryPageInputHandler(MenuWidgetNode *root, u32 flags);
void Menu_DrawUsableItemActionList(int arg0);

void Menu_CreateInventoryListView(MenuWidgetNode *parent) {
    MenuWidgetNode *root;
    MenuWidgetNode *node;
    s32 flags;
    s32 is_layout_locked;

    root = MenuWidget_CreateSimpleNode(6, parent, 0, 0);
    node = MenuWidget_CreateNode(6, root, root);
    root->update = (void (*)())Menu_InventoryPageInputHandler;
    root->disabled = 1;
    node->draw = Menu_DrawUsableItemActionList;
    asm("" : "=m"(*node) : "m"(*node));
    flags = node->layout_flags;
    is_layout_locked = g_MenuLayoutLocked;
    flags |= 0x80;
    node->layout_flags = flags;
    if (is_layout_locked != 0) {
        MenuWidget_ClearColumnLayout(node);
    } else if (g_InvItemUsableFlag == 0) {
        MenuWidget_SetColumnLayout(node, 0x14);
    }
    if (node->cursor_x >= 0) {
        MenuWidget_SetCurrentNode(node);
    }
}
