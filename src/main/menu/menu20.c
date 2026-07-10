/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"
#include "../../../tools/m2c/m2c_macros.h"

typedef int s32;
typedef unsigned int u32;

int g_MenuEquipMode;

int g_MenuActiveWidget;
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

void Menu_DrawEquipSlotListItem(int arg0);
void MenuWidget_DrawList(int arg0, void (*callback)(int));
void Draw_SetTextDimmed(int value);
void Draw_AllocSprite(int arg0);
void Draw_OffsetCursor(int x, int y);
void Inv_ResetActiveList(void);
void Inv_SetActiveList(int arg0, int arg1);
void MenuWidget_InitPool(int arg0);
int MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
void *MenuWidget_CreateSimpleNode(int mode, void *parent, int arg2, int arg3);
void *MenuWidget_CreateNode(int mode, void *parent, void *selected_base);
void MenuWidget_SetCurrentNode(void *node);
int Menu_GetBattleEquipMode(void);
void Draw_SetPrimCallback(void *node, int value);
void Menu_CreateBonusPointAllocationView(void);
void Menu_CreateContextHelpPanel(void);
void MenuWidget_NavScrollTo(int arg0);
void *MenuWidget_GetCurrentNode(void);
void Inv_RebuildSelectableMask(void);
int Inv_GetAyaSlotLimit(void);

extern M2C_UNK Menu_StepItemSelectScreen[];
#define Menu_StepItemSelectScreen (Menu_StepItemSelectScreen[0])
extern M2C_UNK Menu_DrawItemList[];
#define Menu_DrawItemList (Menu_DrawItemList[0])
extern M2C_UNK Menu_DrawAmmoTypeHeader[];
#define Menu_DrawAmmoTypeHeader (Menu_DrawAmmoTypeHeader[0])
extern M2C_UNK Menu_InventoryInputHandler[];
#define Menu_InventoryInputHandler (Menu_InventoryInputHandler[0])
extern M2C_UNK Menu_DrawSelectableEquipSlotList[];
#define Menu_DrawSelectableEquipSlotList (Menu_DrawSelectableEquipSlotList[0])
extern M2C_UNK Inv_SwapSlots[];
#define Inv_SwapSlots (Inv_SwapSlots[0])
extern M2C_UNK Menu_RebuildSelectableMask[];
#define Menu_RebuildSelectableMask (Menu_RebuildSelectableMask[0])

int Menu_GetEquipMode(void) {
    return g_MenuEquipMode;
}

void Menu_OpenInventoryScreen(void) {
    register s32 count asm("$5");
    register s32 bits asm("$3");
    register s32 loop_counter asm("$4");
    register s32 equip_entry asm("$3");
    register s32 raw asm("$2");
    void *root;
    void *node;
    void *current;

    Inv_ResetActiveList();
    g_MenuSelectionLocked = 0;

    if (g_MenuEquipSwapSource != 0) {
        MenuWidget_InitPool(9);

        if (MenuWidget_FindByModeAndSelectedBase(1, 0) == 0) {
            root = MenuWidget_CreateSimpleNode(0, 0, 0, 0);
            node = MenuWidget_CreateNode(0, root, root);
            M2C_FIELD(root, M2C_UNK **, 0x2C) = &Menu_StepItemSelectScreen;
            M2C_FIELD(node, M2C_UNK **, 0x30) = &Menu_DrawItemList;
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
        M2C_FIELD(MenuWidget_CreateSimpleNode(0x1B, 0, 0, 0), M2C_UNK **, 0x30) = &Menu_DrawAmmoTypeHeader;
        root = MenuWidget_CreateSimpleNode(1, current, 0, 0);
        node = MenuWidget_CreateNode(1, root, root);
        M2C_FIELD(root, M2C_UNK **, 0x2C) = &Menu_InventoryInputHandler;
        M2C_FIELD(node, M2C_UNK **, 0x30) = &Menu_DrawSelectableEquipSlotList;
        MenuWidget_SetCurrentNode(node);
        M2C_FIELD(node, M2C_UNK **, 0x84) = &Inv_SwapSlots;
        M2C_FIELD(node, M2C_UNK **, 0x88) = &Menu_RebuildSelectableMask;
        Inv_RebuildSelectableMask();

        g_InvSwapTargetIndex = -1;
        g_InvSelectedItemIndex = -1;
        Draw_SetPrimCallback(node, Inv_GetAyaSlotLimit());
        g_MenuActionSubmenuOpen = 0;

        if (g_MenuEquipSwapSource != 0) {
            equip_entry = g_MenuEquipSwapSource - 1;
            M2C_FIELD(node, s32 *, 0x44) = equip_entry & 1;
            M2C_FIELD(node, s32 *, 0x48) = (equip_entry >> 1) & 0x7F;
            M2C_FIELD(node, s32 *, 0x5C) = equip_entry >> 8;
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
