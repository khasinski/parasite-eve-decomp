/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void *MenuWidget_FindByModeAndSelectedBase();
M2C_UNK MenuWidget_SetCurrentNode();
M2C_UNK Menu_CreateContextHelpPanel();
s32 Inv_GetAyaSlotLimit();
M2C_UNK Inv_RebuildSelectableMask();
M2C_UNK Inv_SortSlotsByPriority();
M2C_UNK Menu_SetBattleEquipMode();
M2C_UNK Queue_Init();
void *MenuWidget_CreateSimpleNode();
void *MenuWidget_CreateNode();
M2C_UNK Draw_SetPrimCallback();
extern s32 g_MenuSelectionLocked;
extern s32 g_MenuActionSubmenuOpen;
extern s32 D_8009CF5C;
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

s32 Menu_OpenEquipSlotSwap(s32 arg0) {
    s32 temp_s2;
    s32 temp_v1;
    void *temp_a0;
    void *temp_s1;
    void *temp_v0;

    temp_s2 = MenuWidget_FindByModeAndSelectedBase(1, 1) != NULL;
    if (temp_s2 == 0) {
        Queue_Init();
        M2C_FIELD(MenuWidget_CreateSimpleNode(0x1B, 0, 0, 0), M2C_UNK **, 0x30) = &Menu_DrawAmmoTypeHeader;
        temp_v0 = MenuWidget_CreateSimpleNode(1, 0, 0, 0);
        temp_s1 = MenuWidget_CreateNode(1, temp_v0, temp_v0);
        temp_a0 = temp_s1;
        M2C_FIELD(temp_v0, M2C_UNK **, 0x2C) = &Menu_InventoryInputHandler;
        M2C_FIELD(temp_s1, M2C_UNK **, 0x30) = &Menu_DrawSelectableEquipSlotList;
        MenuWidget_SetCurrentNode(temp_a0);
        M2C_FIELD(temp_s1, M2C_UNK **, 0x84) = &Inv_SwapSlots;
        M2C_FIELD(temp_s1, M2C_UNK **, 0x88) = &Menu_RebuildSelectableMask;
        Inv_RebuildSelectableMask();
        g_InvSwapTargetIndex = -1;
        g_InvSelectedItemIndex = -1;
        Draw_SetPrimCallback(temp_s1, Inv_GetAyaSlotLimit());
        g_MenuActionSubmenuOpen = 0;
        if (g_MenuEquipSwapSource != 0) {
            temp_v1 = g_MenuEquipSwapSource - 1;
            M2C_FIELD(temp_s1, s32 *, 0x44) = (s32) (temp_v1 & 1);
            M2C_FIELD(temp_s1, s32 *, 0x48) = (s32) ((temp_v1 >> 1) & 0x7F);
            M2C_FIELD(temp_s1, s32 *, 0x5C) = (s32) (temp_v1 >> 8);
        }
        Menu_CreateContextHelpPanel();
        M2C_FIELD(MenuWidget_FindByModeAndSelectedBase(2, 1), s32 *, 0x84) = 0;
        Inv_SortSlotsByPriority();
        g_MenuActionSubmenuOpen = 1;
        D_8009CF5C = arg0;
        g_MenuSelectionLocked = 0;
        Menu_SetBattleEquipMode(0);
    }
    return temp_s2;
}
