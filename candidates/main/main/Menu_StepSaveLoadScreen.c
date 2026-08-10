/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/menu_widget.h"

extern int D_8009CF00;
extern int D_8009CF8C;
extern int D_8009CF94;

MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, MenuWidgetNode *parent, int arg2, int arg3);
MenuWidgetNode *MenuWidget_CreateNode(int mode, MenuWidgetNode *parent, MenuWidgetNode *sibling);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
void MenuWidget_ClearColumnLayout(MenuWidgetNode *node);
void MenuWidget_ClampCursor(MenuWidgetNode *node, int x, int y);
void Menu_CreateContextHelpPanel(void);
void Draw_SetPrimCallback();

int Menu_SaveLoadNavigate(MenuWidgetNode *node, unsigned int flags);
int Menu_InventoryInputHandler(MenuWidgetNode *node, unsigned int flags);
void Menu_DrawArmorListContainer(int arg0);
void Menu_DrawArmorSelectionList(int arg0);
void Menu_DrawSelectableEquipSlotList(int arg0);
void Menu_RebuildSelectableMask(void);
int Menu_GetBattleCount(void);
int Inv_GetAyaSlotLimit(void);
int Inv_FindFirstEmptySlot(int required_slots);
void Inv_TransferItem(void);

void Menu_StepSaveLoadScreen(void) {
    MenuWidgetNode *root_parent;
    MenuWidgetNode *root_node;
    MenuWidgetNode *list_parent;
    MenuWidgetNode *list_node;
    MenuWidgetNode *slot_parent;
    MenuWidgetNode *slot_node;
    int first_empty;
    int battle_count;

    root_parent = MenuWidget_CreateSimpleNode(0xC, 0, 0, 0);
    root_node = MenuWidget_CreateNode(0xC, root_parent, root_parent);
    root_parent->update = (void (*)())Menu_SaveLoadNavigate;
    root_node->field_30 = (void (*)())Menu_DrawArmorListContainer;
    MenuWidget_SetCurrentNode(root_node);

    list_parent = MenuWidget_CreateSimpleNode(0xD, root_node, 0, 0);
    list_node = MenuWidget_CreateNode(0xD, list_parent, list_parent);
    list_parent->field_40 = 1;
    list_parent->update = (void (*)())Menu_InventoryInputHandler;
    list_node->field_30 = (void (*)())Menu_DrawArmorSelectionList;
    list_node->field_84 = (int)Inv_TransferItem;
    MenuWidget_ClearColumnLayout(list_node);
    list_node->field_64 &= -5;
    Draw_SetPrimCallback(list_node, Menu_GetBattleCount());

    slot_parent = MenuWidget_CreateSimpleNode(0xE, list_node, 0, 0);
    slot_node = MenuWidget_CreateNode(0xE, slot_parent, slot_parent);
    slot_parent->update = (void (*)())Menu_InventoryInputHandler;
    slot_node->field_30 = (void (*)())Menu_DrawSelectableEquipSlotList;
    slot_node->field_88 = (int)Menu_RebuildSelectableMask;
    slot_node->field_84 = (int)Inv_TransferItem;
    slot_node->cursor_x = -1;
    D_8009CF94 = -1;
    D_8009CF8C = -1;
    Draw_SetPrimCallback(slot_node, Inv_GetAyaSlotLimit());

    slot_node->field_78 = list_node;
    list_node->field_7C = slot_node;

    battle_count = Menu_GetBattleCount();
    first_empty = Inv_FindFirstEmptySlot(battle_count);
    if (first_empty >= 0) {
        MenuWidget_ClampCursor(slot_node, 0, first_empty + battle_count - 1);
    } else {
        first_empty = Inv_FindFirstEmptySlot(1);
        if (first_empty >= 0) {
            MenuWidget_ClampCursor(slot_node, 0, first_empty + 7);
        }
    }

    slot_node->cursor_x = -1;
    Menu_CreateContextHelpPanel();
    D_8009CF00 = 0;
}
