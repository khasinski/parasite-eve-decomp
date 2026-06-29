/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 MenuWidget_FindByModeAndSelectedBase(M2C_UNK, M2C_UNK); /* extern */
M2C_UNK Inv_SelectActiveList(s32);                         /* extern */
s32 Inv_GetAyaSlotLimit();                                /* extern */
M2C_UNK Inv_RebuildSelectableMask();                            /* extern */
M2C_UNK Inv_RemoveActiveListItem(s32);                         /* extern */
s32 Inv_TransferItemAlt2(M2C_UNK);                         /* extern */
M2C_UNK MenuWidget_DestroyNode(s32);                         /* extern */
M2C_UNK Draw_SetPrimCallback(s32, s32);                    /* extern */
extern s32 g_MenuActiveItemSlot;
extern s32 g_MenuEquipMode;
extern s32 g_MenuActiveItemList;

void Menu_OnItemDiscardConfirm(void *arg0, s32 arg1) {
    s32 temp_v0;
    s32 temp_v0_2;

    if (arg1 != 0) {
        Inv_SelectActiveList(g_MenuActiveItemList);
        Inv_RemoveActiveListItem(g_MenuActiveItemSlot);
        Inv_RebuildSelectableMask();
        MenuWidget_DestroyNode(M2C_FIELD(M2C_FIELD(arg0, void **, 4), s32 *, 4));
        temp_v0 = MenuWidget_FindByModeAndSelectedBase(2, 1);
        if (temp_v0 != 0) {
            Draw_SetPrimCallback(temp_v0, Inv_GetAyaSlotLimit());
        }
        if (g_MenuEquipMode == 2) {
            temp_v0_2 = MenuWidget_FindByModeAndSelectedBase(2, 0x33);
            if (temp_v0_2 != 0) {
                Draw_SetPrimCallback(temp_v0_2, Inv_TransferItemAlt2(0x3803FE));
            }
        }
    }
}
