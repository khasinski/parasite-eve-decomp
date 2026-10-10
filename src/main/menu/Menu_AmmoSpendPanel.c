#include "common.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_inventory.h"
#include "pe1/inventory.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "include_asm.h"

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

void Inv_SelectActiveList(s32 useOverride);
void Inv_ClearSelectionBitset(void);

#define ITEM_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(ItemDataRecord, member)))

extern s32 g_InvAmmoSpendActiveList;
extern s32 g_InvSelectedItemIndex;
extern s32 g_InvSwapSourceList;
extern s32 g_InvSwapTargetIndex;
extern s32 g_MenuSpendArrowDirection;

int Menu_AmmoSpendInputHandler(s32 arg0, s32 arg1);
void Menu_DrawAmmoSpendPanel(void);

void Menu_CreateAmmoSpendPanel(s32 parent) {
    MenuWidgetNode *node;
    void *item;

    node = MenuWidget_CreateSimpleNode(0x3E, parent, 0, 1);
    node->update = (void (*)())Menu_AmmoSpendInputHandler;
    node->draw = Menu_DrawAmmoSpendPanel;
    node->flags = 1;
    MenuWidget_SetCurrentNode(node);
    Inv_SelectActiveList(g_InvAmmoSpendActiveList);
    item = Inv_LookupActiveListData(g_InvSelectedItemIndex);
    if ((item != NULL) && ((ITEM_FIELD(item, u8 *, kind) - 0x13) >= 3U)) {
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

#undef ITEM_FIELD
#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 Spend_Ammo();
s32 func_8005E120();
extern s32 g_InvSelectedItemIndex;
extern s32 g_InvSwapTargetIndex;
extern s32 g_MenuSpendArrowDirection;

s32 Menu_AmmoSpendInputHandler(s32 arg0, s32 arg1) {
    if (arg1 & 0x1000) {
        if (Spend_Ammo(-func_8005E120()) != 0) {
            Menu_PlayErrorSound();
        } else {
            Menu_PlayMoveSound();
        }
        g_MenuSpendArrowDirection = 1;
    } else if (arg1 & 0x4000) {
        if (Spend_Ammo(func_8005E120()) != 0) {
            Menu_PlayErrorSound();
        } else {
            Menu_PlayMoveSound();
        }
        g_MenuSpendArrowDirection = 0;
    } else {
        if (arg1 & 0x10000) {
            Inv_StepScrollDisplay();
            g_InvSwapTargetIndex = -1;
            g_InvSelectedItemIndex = -1;
            Inv_RebuildSelectableMask();
            MenuWidget_DestroyNode(arg0);
            Menu_PlayConfirmSound();
        }
    }
    if (arg1 & 0x40) {
        g_InvSwapTargetIndex = -1;
        g_InvSelectedItemIndex = -1;
        Inv_RebuildSelectableMask();
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }
    return 1;
}


int g_MenuSpendArrowDirection;

void Draw_OffsetCursor(int x, int y);
void Draw_AllocSprite(int arg0);
void Menu_DrawWeaponComparisonPanel(void);

void Menu_DrawAmmoSpendPanel(void) {
    Draw_OffsetCursor(0x3C, 0x12);
    Draw_AllocSprite(g_MenuSpendArrowDirection + 0x4D);
    Draw_OffsetCursor(-0x38, -0xE);
    Menu_DrawWeaponComparisonPanel();
}
