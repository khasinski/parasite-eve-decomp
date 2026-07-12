/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "include_asm.h"
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

void MenuWidget_SetCurrentNode(void *node);
void Inv_SelectActiveList(s32 useOverride);
void *Inv_LookupActiveListData(s32 index);
void Inv_ClearSelectionBitset(void);
void Inv_BuildDisplayFromList(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void *MenuWidget_CreateSimpleNode(s32 mode, s32 arg1, s32 arg2, s32 arg3);

extern s32 g_InvAmmoSpendActiveList;
extern s32 g_InvSelectedItemIndex;
extern s32 g_InvSwapSourceList;
extern s32 g_InvSwapTargetIndex;
extern s32 g_MenuSpendArrowDirection;

void Menu_AmmoSpendInputHandler(void);
void Menu_DrawAmmoSpendPanel(void);

void Menu_CreateAmmoSpendPanel(s32 parent) {
    void *node;
    void *item;

    node = MenuWidget_CreateSimpleNode(0x3E, parent, 0, 1);
    M2C_FIELD(node, void **, 0x2C) = Menu_AmmoSpendInputHandler;
    M2C_FIELD(node, void **, 0x30) = Menu_DrawAmmoSpendPanel;
    M2C_FIELD(node, s32 *, 0x28) = 1;
    MenuWidget_SetCurrentNode(node);
    Inv_SelectActiveList(g_InvAmmoSpendActiveList);
    item = Inv_LookupActiveListData(g_InvSelectedItemIndex);
    if ((item != NULL) && ((u32)(M2C_FIELD(item, u8 *, 6) - 0x13) >= 3U)) {
        s32 active = g_InvAmmoSpendActiveList;
        s32 source = g_InvSwapSourceList;
        s32 selected = g_InvSelectedItemIndex;
        s32 target = g_InvSwapTargetIndex;

        active ^= source;
        source ^= active;
        g_InvAmmoSpendActiveList = active;
        active ^= source;

        selected ^= target;
        target ^= selected;
        g_InvSelectedItemIndex = selected;
        selected ^= target;

        g_InvSwapSourceList = source;
        g_InvAmmoSpendActiveList = active;
        g_InvSwapTargetIndex = target;
        g_InvSelectedItemIndex = selected;
    }
    Inv_BuildDisplayFromList(g_InvAmmoSpendActiveList, g_InvSelectedItemIndex, g_InvSwapSourceList, g_InvSwapTargetIndex);
    Inv_ClearSelectionBitset();
    g_MenuSpendArrowDirection = 0;
}
