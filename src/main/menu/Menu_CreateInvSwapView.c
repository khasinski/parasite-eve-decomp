#include "pe1/menu_inventory.h"
#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --dont-expand-li */
/* ASPSX_VERSION: 2.21 */


void MenuWidget_SetCursorY(MenuWidgetNode *node);
void Menu_EquipOptionsInputHandler(void);
void Menu_DrawEquipOptionsList(void);

extern u32 D_8009CF0C;
extern u32 D_8009CFB8;

void Menu_CreateInvSwapView(MenuWidgetNode *parent, int mode) {
    int mode_offset;
    MenuWidgetNode *node;
    MenuWidgetNode *current;

    mode_offset = mode;
    node = MenuWidget_CreateSimpleNode(mode_offset + 0x3A, parent, 0, 0);
    current = MenuWidget_CreateNode(mode_offset + 0x3A, node, node);
    node->update = Menu_EquipOptionsInputHandler;
    current->draw = Menu_DrawEquipOptionsList;
    MenuWidget_SetCurrentNode(current);
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x33));

    if (mode_offset != 0) {
        u32 value;

        value = D_8009CF0C;
        D_8009CFB8 = value;
    } else {
        D_8009CFB8 = 0;
    }
}
