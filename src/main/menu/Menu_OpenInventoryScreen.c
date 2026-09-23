#include "common.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

extern s32 D_8009CF0C;
extern s32 g_MenuItemContextFlag;
extern s32 g_MenuBattleStatusOverlayActive;
extern s32 g_MenuSelectionLocked;
extern s32 g_MenuActionSubmenuOpen;
extern s32 g_InvSelectedItemIndex;
extern s32 g_InvSwapTargetIndex;
extern s32 g_MenuEquipSwapSource;
extern s32 D_8009CF9C;
extern s32 D_8009D008;
extern short D_800C0E46[];

void Inv_ResetActiveList(void);
void Inv_SetActiveList(int arg0, int arg1);
/* The retail caller supplies 9 although the pool initializer ignores it. */
void MenuWidget_InitPool();
int MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, MenuWidgetNode *parent, int arg2, int arg3);
MenuWidgetNode *MenuWidget_CreateNode(int mode, MenuWidgetNode *parent, MenuWidgetNode *selected_base);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
int Menu_GetBattleEquipMode(void);
void Menu_CreateBonusPointAllocationView(void);
void Menu_CreateContextHelpPanel(void);
void MenuWidget_NavScrollTo(int arg0);
MenuWidgetNode *MenuWidget_GetCurrentNode(void);
void Inv_RebuildSelectableMask(void);
int Inv_GetAyaSlotLimit(void);

extern int Menu_StepItemSelectScreen(MenuWidgetNode *node, unsigned int flags);
extern void Menu_DrawItemList(void *node);
extern void Menu_DrawAmmoTypeHeader(MenuWidgetNode *node);
extern int Menu_InventoryInputHandler(MenuWidgetNode *node, unsigned int flags);
extern void Menu_DrawSelectableEquipSlotList(int node);
extern int Inv_SwapSlots(int unused, int from, int unused2, int to);
extern void Menu_RebuildSelectableMask(void);

void Menu_OpenInventoryScreen(void) {
    s32 count;
    s32 bits;
    s32 loop_counter;
    s32 equip_entry;
    s32 raw;
    MenuWidgetNode *root0;
    MenuWidgetNode *root1;
    MenuWidgetNode *node;
    MenuWidgetNode *current;
    register s32 nine asm("$4");

    nine = 9;
    Inv_ResetActiveList();
    D_8009CF0C = 0;

    if (g_MenuEquipSwapSource != 0) {
        MenuWidget_InitPool(nine);

        if (MenuWidget_FindByModeAndSelectedBase(1, 0) == 0) {
            root0 = MenuWidget_CreateSimpleNode(0, 0, 0, 0);
            node = MenuWidget_CreateNode(0, root0, root0);
            root0->update = (void (*)())Menu_StepItemSelectScreen;
            node->draw = (void (*)())Menu_DrawItemList;
            MenuWidget_SetCurrentNode(node);

            if (Menu_GetBattleEquipMode() != 0) {
                raw = g_MenuItemContextFlag;
                bits = raw & 0x1F;
            } else {
                raw = g_MenuItemContextFlag;
                bits = raw & 0x1EF;
            }

            count = 0;
            loop_counter = 8;
            do {
                count += bits & 1;
                loop_counter--;
                bits >>= 1;
            } while (loop_counter >= 0);

            Draw_SetPrimCallback(node, count);
            g_MenuSelectionLocked = 0;
            g_MenuBattleStatusOverlayActive = 1;
            Menu_CreateBonusPointAllocationView();
            Menu_CreateContextHelpPanel();
        }

        MenuWidget_NavScrollTo(0x2D);
        MenuWidget_NavScrollTo(0x18);
        MenuWidget_NavScrollTo(0x12);

        current = MenuWidget_GetCurrentNode();
        MenuWidget_CreateSimpleNode(0x1B, 0, 0, 0)->draw = (void (*)())Menu_DrawAmmoTypeHeader;
        root1 = MenuWidget_CreateSimpleNode(1, current, 0, 0);
        node = MenuWidget_CreateNode(1, root1, root1);
        root1->update = (void (*)())Menu_InventoryInputHandler;
        node->draw = (void (*)())Menu_DrawSelectableEquipSlotList;
        MenuWidget_SetCurrentNode(node);
        node->itemAction = (MenuWidgetItemAction)Inv_SwapSlots;
        node->refreshItems = Menu_RebuildSelectableMask;
        Inv_RebuildSelectableMask();

        g_InvSwapTargetIndex = -1;
        g_InvSelectedItemIndex = -1;
        Draw_SetPrimCallback(node, Inv_GetAyaSlotLimit());
        g_MenuActionSubmenuOpen = 0;

        if (g_MenuEquipSwapSource != 0) {
            equip_entry = g_MenuEquipSwapSource - 1;
            node->cursor_x = equip_entry & 1;
            node->cursor_y = (equip_entry >> 1) & 0x7F;
            node->scroll_y = equip_entry >> 8;
        }

        if (D_8009D008 != 0) {
            D_8009D008 = 0;
        } else {
            D_800C0E46[g_MenuEquipSwapSource] = D_8009CF9C;
            Inv_RebuildSelectableMask();
        }

        g_MenuEquipSwapSource = 0;
    } else {
        Inv_SetActiveList(9, 0);
    }
}
