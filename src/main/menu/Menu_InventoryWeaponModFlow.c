#include "common.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK MenuWidget_ClearCursorY();
s32 MenuWidget_FindByModeAndSelectedBase();
void *MenuWidget_GetCurrentNode();
M2C_UNK MenuWidget_SetCurrentNode();
M2C_UNK Menu_CreateBonusPointAllocationView();
s32 Inv_GetAyaSlotLimit();
M2C_UNK Inv_RebuildSelectableMask();
void *MenuWidget_CreateSimpleNode();
M2C_UNK MenuWidget_NavScrollTo();
void *MenuWidget_CreateNode();
extern s32 g_MenuActionSubmenuOpen;
extern s32 g_InvSelectedItemIndex;
extern s32 g_InvSwapTargetIndex;
extern s32 g_MenuEquipSwapSource;
extern s32 g_MenuInventoryViewMode;
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

void Menu_OpenInventoryOrSwapView(s32 arg0) {
    s32 mode;
    register s32 temp_v1 asm("$3");
    void *temp_a0;
    void *temp_s1;
    void *temp_v0;
    void *temp_v0_2;

    MenuWidget_NavScrollTo(0x3C);
    MenuWidget_NavScrollTo(0x3B);
    MenuWidget_NavScrollTo(0x3A);
    mode = g_MenuInventoryViewMode;
    if (mode == 1) {
        goto clear_cursor;
    }
    if (mode >= 2) {
        goto check_two;
    }
    if (mode == 0) {
        goto case_zero;
    }
    return;
check_two:
    if (mode == 2) {
        goto clear_cursor;
    }
    return;
case_zero:
    if (arg0 != 0) {
        temp_v0 = MenuWidget_GetCurrentNode();
        M2C_FIELD(temp_v0, s32 *, 0x48) = 0;
        M2C_FIELD(MenuWidget_CreateSimpleNode(0x1B, NULL, 0, 0), M2C_UNK **, 0x30) = &Menu_DrawAmmoTypeHeader;
        temp_v0_2 = MenuWidget_CreateSimpleNode(1, temp_v0, 0, 0);
        temp_s1 = MenuWidget_CreateNode(1, temp_v0_2, temp_v0_2);
        temp_a0 = temp_s1;
        M2C_FIELD(temp_v0_2, M2C_UNK **, 0x2C) = &Menu_InventoryInputHandler;
        M2C_FIELD(temp_s1, M2C_UNK **, 0x30) = &Menu_DrawSelectableEquipSlotList;
        MenuWidget_SetCurrentNode(temp_a0);
        M2C_FIELD(temp_s1, M2C_UNK **, 0x84) = &Inv_SwapSlots;
        M2C_FIELD(temp_s1, M2C_UNK **, 0x88) = &Menu_RebuildSelectableMask;
        Inv_RebuildSelectableMask();
        g_InvSwapTargetIndex = -1;
        g_InvSelectedItemIndex = -1;
        Draw_SetPrimCallback(temp_s1, Inv_GetAyaSlotLimit());
        temp_v1 = g_MenuEquipSwapSource;
        g_MenuActionSubmenuOpen = 0;
        if (temp_v1 != 0) {
            temp_v1 -= 1;
            M2C_FIELD(temp_s1, s32 *, 0x44) = (temp_v1 & 1);
            M2C_FIELD(temp_s1, s32 *, 0x48) = ((temp_v1 >> 1) & 0x7F);
            M2C_FIELD(temp_s1, s32 *, 0x5C) = (temp_v1 >> 8);
            return;
        }
        return;
    }
    Menu_CreateBonusPointAllocationView();
    return;
clear_cursor:
    MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x33));
}
#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void Draw_OffsetCursor(int, int);
M2C_UNK Draw_AllocSprite();
extern u8 D_800922B8[];
extern u8 D_800922BC[];
extern u8 D_800922C4[];
extern u8 D_800922CC[];
extern s32 g_MenuInventoryViewMode;

void Menu_DrawWeaponModSlots(s32 arg0) {
    s32 temp_s0;
    u8 var_a0;

    temp_s0 = arg0 + D_800922B8[g_MenuInventoryViewMode];
    if (g_MenuInventoryViewMode == 1) {
        Draw_OffsetCursor(0, 2);
        Draw_AllocSprite(D_800922BC[temp_s0]);
        Draw_OffsetCursor(0x12, 2);
        Draw_AllocSprite(0x22U);
        Draw_OffsetCursor(0xC, -2);
        Draw_AllocSprite(D_800922C4[temp_s0]);
        if (g_MenuInventoryViewMode == 0) {
            Draw_OffsetCursor(0x12, 2);
            Draw_AllocSprite(0x22U);
            Draw_OffsetCursor(0xC, -2);
            var_a0 = D_800922CC[temp_s0];
            goto block_5;
        }
    } else {
        Draw_AllocSprite(D_800922BC[temp_s0]);
        Draw_AllocSprite(0x68U);
        Draw_OffsetCursor(0x12, 4);
        Draw_AllocSprite(0x22U);
        Draw_OffsetCursor(0xC, -4);
        Draw_AllocSprite(D_800922C4[temp_s0]);
        Draw_AllocSprite(0x68U);
        if (g_MenuInventoryViewMode == 0) {
            Draw_OffsetCursor(0x12, 4);
            Draw_AllocSprite(0x22U);
            Draw_OffsetCursor(0xC, -4);
            Draw_AllocSprite(D_800922CC[temp_s0]);
            var_a0 = 0x68;
block_5:
            Draw_AllocSprite(var_a0);
        }
    }
}


void Menu_DrawWeaponModSlots(s32 arg0);
void MenuWidget_DrawList(int arg0, void (*callback)(s32));

void Menu_DrawEquipOptionsList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawWeaponModSlots);
}
#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "include_asm.h"

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK MenuWidget_OffsetPosition();
M2C_UNK MenuWidget_SetCurrentNode();
void Menu_OpenInventoryOrSwapView(s32 arg0);
M2C_UNK Menu_CreateEquipInfoPanel();
M2C_UNK Menu_PlayConfirmSound();
M2C_UNK Menu_PlayCancelSound();
M2C_UNK Sort_ContainerItems();
s32 MenuWidget_GetChild();
void *MenuWidget_CreateSimpleNode();
void *MenuWidget_CreateNode();
s32 MenuWidget_GridCellIndex();
extern s32 g_MenuInventoryViewMode;
extern M2C_UNK Menu_DrawEquipSortToggleList[];
#define Menu_DrawEquipSortToggleList (Menu_DrawEquipSortToggleList[0])
extern M2C_UNK Menu_EquipGridHandler[];
#define Menu_EquipGridHandler (Menu_EquipGridHandler[0])

s32 Menu_EquipOptionsInputHandler(s32 arg0, s32 arg1) {
    s32 temp_a1;
    s32 temp_v0;
    void *temp_v0_2;
    void *temp_v0_3;

    if (arg1 & 0x10000) {
        temp_v0 = MenuWidget_GetChild(arg0, 0);
        temp_a1 = MenuWidget_GridCellIndex(temp_v0);
        switch (g_MenuInventoryViewMode) {                       /* irregular */
        case 0:
            if (temp_a1 == 2) {
                temp_v0_2 = MenuWidget_CreateSimpleNode(0x3B, temp_v0, 0, 0);
                temp_v0_3 = MenuWidget_CreateNode(0x3B, temp_v0_2, temp_v0_2);
                M2C_FIELD(temp_v0_2, M2C_UNK **, 0x2C) = &Menu_EquipGridHandler;
                MenuWidget_OffsetPosition(temp_v0_2, 0x70 - M2C_FIELD(temp_v0_2, s32 *, 0x18), 0x48 - M2C_FIELD(temp_v0_2, s32 *, 0x1C));
                M2C_FIELD(temp_v0_3, M2C_UNK **, 0x30) = &Menu_DrawEquipSortToggleList;
                MenuWidget_SetCurrentNode(temp_v0_3);
            } else {
            case 2:
                Menu_CreateEquipInfoPanel(temp_v0, temp_a1);
            }
            break;
        case 1:
            Sort_ContainerItems(temp_a1, temp_a1);
            Menu_OpenInventoryOrSwapView(1);
            break;
        }
        Menu_PlayConfirmSound();
        return 1;
    }
    if (arg1 & 0x40) {
        Menu_OpenInventoryOrSwapView(0);
        Menu_PlayCancelSound();
    }
    return 1;
}
