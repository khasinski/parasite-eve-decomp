#include "common.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK MenuWidget_SetCurrentNode();
s32 Inv_GetAyaSlotLimit();
M2C_UNK Inv_RebuildSelectableMask();
void *MenuWidget_CreateSimpleNode();
void *MenuWidget_CreateNode();
extern s32 g_MenuActionSubmenuOpen;
extern s32 g_InvSelectedItemIndex;
extern s32 g_InvSwapTargetIndex;
extern s32 g_MenuEquipSwapSource;
extern M2C_UNK Menu_InventoryInputHandler[];
#define Menu_InventoryInputHandler (Menu_InventoryInputHandler[0])
extern M2C_UNK Menu_DrawAmmoTypeHeader[];
#define Menu_DrawAmmoTypeHeader (Menu_DrawAmmoTypeHeader[0])
extern M2C_UNK Menu_DrawSelectableEquipSlotList[];
#define Menu_DrawSelectableEquipSlotList (Menu_DrawSelectableEquipSlotList[0])
extern M2C_UNK Menu_RebuildSelectableMask[];
#define Menu_RebuildSelectableMask (Menu_RebuildSelectableMask[0])
extern M2C_UNK Inv_SwapSlots[];
#define Inv_SwapSlots (Inv_SwapSlots[0])

void Menu_CreateEquipScreen(s32 arg0) {
    s32 entryIndex;
    void *currentNode;
    void *node;
    void *parent;

    M2C_FIELD(MenuWidget_CreateSimpleNode(0x1B, 0, 0, 0), M2C_UNK **, 0x30) = &Menu_DrawAmmoTypeHeader;
    parent = MenuWidget_CreateSimpleNode(1, arg0, 0, 0);
    node = MenuWidget_CreateNode(1, parent, parent);
    currentNode = node;
    M2C_FIELD(parent, M2C_UNK **, 0x2C) = &Menu_InventoryInputHandler;
    M2C_FIELD(node, M2C_UNK **, 0x30) = &Menu_DrawSelectableEquipSlotList;
    MenuWidget_SetCurrentNode(currentNode);
    M2C_FIELD(node, M2C_UNK **, 0x84) = &Inv_SwapSlots;
    M2C_FIELD(node, M2C_UNK **, 0x88) = &Menu_RebuildSelectableMask;
    Inv_RebuildSelectableMask();
    g_InvSwapTargetIndex = -1;
    g_InvSelectedItemIndex = -1;
    Draw_SetPrimCallback(node, Inv_GetAyaSlotLimit());
    g_MenuActionSubmenuOpen = 0;
    if (g_MenuEquipSwapSource != 0) {
        entryIndex = g_MenuEquipSwapSource - 1;
        M2C_FIELD(node, s32 *, 0x44) = (s32) (entryIndex & 1);
        M2C_FIELD(node, s32 *, 0x48) = (s32) ((entryIndex >> 1) & 0x7F);
        M2C_FIELD(node, s32 *, 0x5C) = (s32) (entryIndex >> 8);
    }
}
#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void *MenuWidget_FindByModeAndSelectedBase();
s32 MenuWidget_GetCurrentNode();
M2C_UNK MenuWidget_SetCurrentNode();
M2C_UNK Menu_CreateAmmoSpendPanel();
M2C_UNK Inv_SelectActiveList();
s32 func_80052F0C();
s32 Inv_GetAyaSlotLimit();
M2C_UNK Inv_RebuildSelectableMask();
s32 Inv_TestSelectionBit();
M2C_UNK Inv_InitSlotDisplay(void *, void *);
s32 Inv_BuildCompatibleWeaponBitset();
s32 MenuWidget_GridCellIndex();
extern s32 g_InvAmmoSpendActiveList;
extern s32 g_InvSelectedItemIndex;
extern s32 g_InvSwapSourceList;

void Menu_OnInventoryItemConfirm(s32 arg0) {
    s32 temp_v0;
    s32 var_s0;
    s32 var_s0_2;
    void *temp_s0;
    void *temp_s0_2;
    void *temp_s1;
    void *temp_s1_2;

    g_InvAmmoSpendActiveList = func_80052F0C();
    g_InvSelectedItemIndex = arg0;
    temp_v0 = Inv_BuildCompatibleWeaponBitset(arg0);
    if (temp_v0 == 0) {
        g_InvSelectedItemIndex = -1;
        Inv_RebuildSelectableMask();
        return;
    }
    if (temp_v0 == 1) {
        temp_s0 = MenuWidget_FindByModeAndSelectedBase(2, 0xD);
        temp_s1 = MenuWidget_FindByModeAndSelectedBase(2, 0xE);
        if ((temp_s0 != NULL) && (MenuWidget_GridCellIndex(temp_s0) != 0) && (temp_s1 != NULL)) {
            M2C_FIELD(temp_s0, s32 *, 0x44) = -1;
            Inv_SelectActiveList(0);
            var_s0 = 0;
loop_7:
            if ((var_s0 < Inv_GetAyaSlotLimit()) && (Inv_TestSelectionBit(var_s0) == 0)) {
                var_s0 += 1;
                goto loop_7;
            }
            M2C_FIELD(temp_s1, s32 *, 0x44) = 0;
            M2C_FIELD(temp_s1, s32 *, 0x48) = (s32) (var_s0 & -(var_s0 < Inv_GetAyaSlotLimit()));
            MenuWidget_SetCurrentNode(temp_s1);
        }
        {
            u8 *cf90_addr;
            u8 *cf94_addr;
            s32 cf88_value;

            cf90_addr = (u8 *)0x800A0000;
            __asm__ volatile("" : "=r"(cf90_addr) : "0"(cf90_addr));
            cf90_addr -= 0x3070;
            cf88_value = g_InvAmmoSpendActiveList;
            __asm__ volatile("" : "=r"(cf88_value) : "0"(cf88_value));
            cf94_addr = (u8 *)0x800A0000;
            __asm__ volatile("" : "=r"(cf94_addr) : "0"(cf94_addr));
            cf94_addr -= 0x306C;
            g_InvSwapSourceList = cf88_value;
            __asm__ volatile("" : : "r"(cf90_addr), "r"(cf94_addr));
            Inv_InitSlotDisplay(cf90_addr, cf94_addr);
        }
        Menu_CreateAmmoSpendPanel(MenuWidget_GetCurrentNode());
        return;
    }
    temp_s0_2 = MenuWidget_FindByModeAndSelectedBase(2, 0xD);
    temp_s1_2 = MenuWidget_FindByModeAndSelectedBase(2, 0xE);
    if ((temp_s0_2 != NULL) && (MenuWidget_GridCellIndex(temp_s0_2) != 0) && (temp_s1_2 != NULL)) {
        M2C_FIELD(temp_s0_2, s32 *, 0x44) = -1;
        Inv_SelectActiveList(0);
        var_s0_2 = 0;
loop_16:
        if ((var_s0_2 < Inv_GetAyaSlotLimit()) && (Inv_TestSelectionBit(var_s0_2) == 0)) {
            var_s0_2 += 1;
            goto loop_16;
        }
        M2C_FIELD(temp_s1_2, s32 *, 0x44) = 0;
        M2C_FIELD(temp_s1_2, s32 *, 0x48) = (s32) (var_s0_2 & -(var_s0_2 < Inv_GetAyaSlotLimit()));
        MenuWidget_SetCurrentNode(temp_s1_2);
    }
}
