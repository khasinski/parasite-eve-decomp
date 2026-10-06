/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/menu_equipment.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_queue.h"
#include "pe1/battle_cmd.h"
#include "common.h"
#include "pe1/menu_widget.h"

/* Builders that open the equip screen (weapon and armor lists) and the
 * inventory swap screen, plus the equip-mode getter. Contiguous at
 * 0x8004E704; Menu_DrawEquipListContainer, which follows, is still a
 * separate file. */

extern int g_MenuEquipMode;
extern int D_8009CF0C;
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

void Menu_OpenEquipScreen(int mode)
{
    MenuWidgetNode *parent, *container, *list;
    int count, offset;

    container = MenuWidget_FindByModeAndSelectedBase(2, 50);
    if (!container) {
        parent = MenuWidget_CreateSimpleNode(50, 0, 0, 0);
        container = MenuWidget_CreateNode(50, parent, parent);
        parent->update = (void (*)())Menu_StepInventoryCategory;
        container->draw = Menu_DrawEquipListContainer;
        if (!mode)
            Draw_SetPrimCallback(container, 3);
        MenuWidget_SetCurrentNode(container);
    }

    parent = MenuWidget_CreateSimpleNode(51, container, 0, 0);
    container = MenuWidget_CreateNode(51, parent, parent);
    parent->update = (void (*)())Menu_InventoryInputHandler;
    parent->disabled = 1;
    container->draw = Menu_DrawWeaponList;
    container->itemAction = Inv_RebuildWithSlotLimit;
    container->cursor_x = -1;
    if (mode)
        D_8009CF0C = 2;
    else
        D_8009CF0C = 1;

    count = Inv_StepScrollDisplay2(mode);
    BattleCmd_SyncActiveAmmo();
    Menu_SetBattleEquipMode(0);
    Draw_SetPrimCallback(container,
        Inv_TransferItemAlt2(mode ? 0x3803FE : 0xF400));

    offset = container->popup_node == 0;
    parent = MenuWidget_CreateSimpleNode(52, container, 0, 0);
    list = MenuWidget_CreateNode(52, parent, parent);
    parent->update = (void (*)())Menu_InventoryInputHandler;
    if (offset)
        MenuWidget_OffsetPosition(parent, -6, 0);
    list->draw = Menu_DrawArmorList;
    list->itemAction = Inv_RebuildWithSlotLimit;
    list->refreshItems = Menu_RebuildSelectableMask;
    list->cursor_x = -1;
    D_8009CF94 = -1;
    D_8009CF8C = -1;
    Draw_SetPrimCallback(list, count);
    list->linkedPrevious = container;
    container->linkedNext = list;

    if (!MenuWidget_FindByModeAndSelectedBase(1, 19)) {
        parent = MenuWidget_CreateSimpleNode(19, 0, 0, 0);
        parent->draw = Menu_DrawContextHelpText;
    }
    D_8009CF00 = 0;
    D_8009CEFC = 0;
    Queue_Init();
}

/* Reopen the equip screen with the current menu mode restored. */
void Menu_ReopenEquipScreen(void) {
    Menu_OpenEquipScreen(g_MenuEquipMode - 1);
}

int Menu_GetEquipMode(void) {
    return D_8009CF0C;
}

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
        /* The retail caller passes 9 although the pool initializer ignores it. */
        ((void (*)())MenuWidget_InitPool)(nine);

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
