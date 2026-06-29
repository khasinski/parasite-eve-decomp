/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK MenuWidget_SetCurrentNode();
s32 Inv_GetAyaSlotLimit();
M2C_UNK Inv_RebuildSelectableMask();
void *MenuWidget_CreateSimpleNode();
void *MenuWidget_CreateNode();
M2C_UNK Draw_SetPrimCallback();
extern s32 g_MenuActionSubmenuOpen;
extern s32 g_InvSelectedItemIndex;
extern s32 g_InvSwapTargetIndex;
extern s32 g_MenuEquipSwapSource;
extern struct { char _[16]; } func_80044444_o __asm__("Menu_InventoryInputHandler");
#define Menu_InventoryInputHandler (*(M2C_UNK *)&func_80044444_o)
extern struct { char _[16]; } func_800447F0_o __asm__("Menu_DrawAmmoTypeHeader");
#define Menu_DrawAmmoTypeHeader (*(M2C_UNK *)&func_800447F0_o)
extern struct { char _[16]; } Menu_DrawSelectableEquipSlotList_o __asm__("func_8004F8D0");
#define Menu_DrawSelectableEquipSlotList (*(M2C_UNK *)&Menu_DrawSelectableEquipSlotList_o)
extern struct { char _[16]; } func_80050260_o __asm__("Menu_RebuildSelectableMask");
#define Menu_RebuildSelectableMask (*(M2C_UNK *)&func_80050260_o)
extern struct { char _[16]; } Inv_SwapSlots_o __asm__("Inv_SwapSlots");
#define Inv_SwapSlots (*(M2C_UNK *)&Inv_SwapSlots_o)

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
