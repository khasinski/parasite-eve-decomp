#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/psyq_nop.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

int Menu_StepInventoryCategory(MenuWidgetNode *node, unsigned int flags)
{
    int selected, mode;
    MenuWidgetNode *child, *parent;
    node = MenuWidget_GetChild(node, 0);
    if (flags & 0x10000) {
        selected = MenuWidget_GridCellIndex(node);
        switch (D_8009CF0C == 1 && selected == 2 ? 4 : selected) {
        case 0:
            node = MenuWidget_FindByModeAndSelectedBase(2, 51);
            node->cursor_x = 0;
            MenuWidget_SetCurrentNode(node);
            Menu_PlayConfirmSound();
            break;
        case 1:
            parent = MenuWidget_CreateSimpleNode(59, node, 0, 0);
            child = MenuWidget_CreateNode(59, parent, parent);
            parent->update = (void (*)())Menu_EquipOptionsInputHandler;
            child->draw = Menu_DrawEquipOptionsList;
            MenuWidget_SetCurrentNode(child);
            MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 51));
            mode = D_8009CF0C;
            PE1_NOP();
            D_8009CFB8 = mode;
            break;
        case 2:
            MenuWidget_NavScrollTo(51);
            MenuWidget_NavScrollTo(52);
            D_8009CF1C = 1;
            Inv_SelectActiveList(0);
            g_StatBaseTable[0] = Inv_CountByCategory(14) ? 999 : Inv_CountByCategory(12);
            g_StatBaseTable[1] = Inv_CountByCategory(15) ? 999 : Inv_CountByCategory(13);
            if (D_8009CF0C) {
                g_StatBaseTable[2] = WayneStorage_CountItemType(14) ? 999 : WayneStorage_CountItemType(12);
                g_StatBaseTable[3] = WayneStorage_CountItemType(15) ? 999 : WayneStorage_CountItemType(13);
            } else {
                g_StatBaseTable[3] = 0;
                g_StatBaseTable[2] = 0;
            }
            Menu_StepInventoryRoot(0x33E, -1, -1);
            Menu_PlayConfirmSound();
            break;
        case 3:
            MenuWidget_NavScrollTo(51);
            MenuWidget_NavScrollTo(52);
            g_MenuItemUseMode = 1;
            Menu_StepInventoryRoot(0x37E, -1, -1);
            Menu_PlayConfirmSound();
            break;
        case 4:
            if (flags & 0x10000) {
                Menu_OpenInventoryScreen();
                Menu_PlayConfirmSound();
            }
            break;
        }
    } else if (flags & 0x40) {
        Menu_OpenInventoryScreen();
        Menu_PlayCancelSound();
    }
    return 1;
}
