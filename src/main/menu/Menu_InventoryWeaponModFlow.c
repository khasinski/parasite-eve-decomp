#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 Inv_GetAyaSlotLimit();
extern s32 g_MenuActionSubmenuOpen;
extern s32 g_InvSelectedItemIndex;
extern s32 g_InvSwapTargetIndex;
extern s32 g_MenuEquipSwapSource;
extern s32 g_MenuInventoryViewMode;
int Menu_InventoryInputHandler(MenuWidgetNode *node, unsigned int flags);
void Menu_DrawAmmoTypeHeader(MenuWidgetNode *node);
void Menu_DrawSelectableEquipSlotList(int node);
void Menu_RebuildSelectableMask(void);
int Inv_SwapSlots(int unused, int from, int unused2, int to);

void Menu_OpenInventoryOrSwapView(s32 arg0) {
    s32 mode;
    register s32 temp_v1 asm("$3");
    MenuWidgetNode *temp_a0;
    MenuWidgetNode *temp_s1;
    MenuWidgetNode *temp_v0;
    MenuWidgetNode *temp_v0_2;

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
        temp_v0->cursor_y = 0;
        M2C_FIELD(MenuWidget_CreateSimpleNode(0x1B, NULL, 0, 0), M2C_UNK **, 0x30) = &Menu_DrawAmmoTypeHeader;
        temp_v0_2 = MenuWidget_CreateSimpleNode(1, temp_v0, 0, 0);
        temp_s1 = MenuWidget_CreateNode(1, temp_v0_2, temp_v0_2);
        temp_a0 = temp_s1;
        temp_v0_2->update = (void (*)())Menu_InventoryInputHandler;
        temp_s1->draw = Menu_DrawSelectableEquipSlotList;
        MenuWidget_SetCurrentNode(temp_a0);
        temp_s1->itemAction = (MenuWidgetItemAction)Inv_SwapSlots;
        temp_s1->refreshItems = Menu_RebuildSelectableMask;
        Inv_RebuildSelectableMask();
        g_InvSwapTargetIndex = -1;
        g_InvSelectedItemIndex = -1;
        Draw_SetPrimCallback(temp_s1, Inv_GetAyaSlotLimit());
        temp_v1 = g_MenuEquipSwapSource;
        g_MenuActionSubmenuOpen = 0;
        if (temp_v1 != 0) {
            temp_v1 -= 1;
            temp_s1->cursor_x = (temp_v1 & 1);
            temp_s1->cursor_y = ((temp_v1 >> 1) & 0x7F);
            temp_s1->scroll_y = (temp_v1 >> 8);
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
void MenuWidget_DrawList(void *arg0, void (*callback)(int));

void Menu_DrawEquipOptionsList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawWeaponModSlots);
}
#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "include_asm.h"

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void Menu_OpenInventoryOrSwapView(s32 arg0);
void Menu_CreateEquipInfoPanel(int arg0, unsigned int arg1);
void Menu_PlayConfirmSound(void);
void Menu_PlayCancelSound(void);
void Sort_ContainerItems(int arg0);
extern s32 g_MenuInventoryViewMode;
void Menu_DrawEquipSortToggleList(int arg0);
int Menu_EquipGridHandler(void *arg0, int arg1);

s32 Menu_EquipOptionsInputHandler(void *arg0, s32 arg1) {
    s32 temp_a1;
    MenuWidgetNode *temp_v0;
    MenuWidgetNode *temp_v0_2;
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
            Sort_ContainerItems(temp_a1);
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
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

void Menu_DrawEquipSortToggleRow(int arg0);
void MenuWidget_DrawList(void *arg0, void (*callback)(int));

int g_MenuInventoryViewMode;

void Sort_ContainerItems(int arg0);
void Inv_SortInventoryByMode(int arg0, int arg1);
void Menu_OpenInventoryOrSwapView(int arg0);
void Menu_PlayConfirmSound(void);
void Menu_PlayCancelSound(void);

int g_InvItemUsableFlag;
int g_MenuInventoryViewMode;

int Menu_EquipSelectInput(void *arg0, unsigned int arg1);
void Menu_DrawEquipInfoOptionList(int arg0);

void Menu_DrawEquipSortToggleList(int arg0) {
        MenuWidget_DrawList(arg0, Menu_DrawEquipSortToggleRow);
}

int Menu_EquipGridHandler(void *arg0, int arg1) {
    int temp;

    if (arg1 & 0x10000) {
        temp = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0));
        if (g_MenuInventoryViewMode != 0) {
            Sort_ContainerItems(temp);
        } else {
            Inv_SortInventoryByMode(2, temp);
        }
        Menu_OpenInventoryOrSwapView(1);
        Menu_PlayConfirmSound();
    } else if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }

    return 1;
}

void Menu_CreateEquipInfoPanel(int arg0, unsigned int arg1) {
    void *node;
    void *child;

    node = MenuWidget_CreateSimpleNode(0x3C, arg0, 0, 0);
    child = MenuWidget_CreateNode(0x3C, node, node);
    *(void **)((char *)node + 0x2C) = Menu_EquipSelectInput;
    *(void **)((char *)child + 0x30) = Menu_DrawEquipInfoOptionList;
    MenuWidget_SetCurrentNode(child);

    if (g_MenuInventoryViewMode == 0) {
        MenuWidget_OffsetPosition(
            node,
            0x70 - *(int *)((char *)node + 0x18),
            0x20 - *(int *)((char *)node + 0x1C)
        );
    }

    g_InvItemUsableFlag = arg1 < 1;
    if (arg1 != 0) {
        MenuWidget_OffsetPosition(node, 0, 0x14);
    }
}
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

void Menu_DrawEquipInfoOptionRow(int arg0);
void MenuWidget_DrawList(void *arg0, void (*callback)(int));

extern int g_InvItemUsableFlag;
extern int g_MenuInventoryViewMode;

extern void Inv_SortInventoryByMode(int arg0, int arg1);
extern void Sort_InventoryItems(int arg0, int arg1);
extern void Menu_OpenInventoryOrSwapView(int arg0);
extern void Menu_PlayConfirmSound(void);
extern void Menu_PlayCancelSound(void);

void Menu_DrawEquipInfoOptionList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawEquipInfoOptionRow);
}

int Menu_EquipSelectInput(void *arg0, unsigned int arg1) {
    int temp;
    void (*handler)(int, int);

    if (arg1 & 0x10000) {
        temp = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0));
        handler = g_MenuInventoryViewMode ? Sort_InventoryItems : Inv_SortInventoryByMode;
        handler(g_InvItemUsableFlag, temp);
        Menu_OpenInventoryOrSwapView(1);
        Menu_PlayConfirmSound();
    } else if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }

    return 1;
}
