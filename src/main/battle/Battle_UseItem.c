/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void *BattleCmd_AllocSlot();                              /* extern */
M2C_UNK Inv_SetActiveList(M2C_UNK, s32 *);              /* extern */
s32 BattleCmd_GetRemainingAmmo(M2C_UNK);                         /* extern */
M2C_UNK BattleCmd_ChangeWeaponAndSync(s32);                         /* extern */
s32 Inv_GetSlotHighlightState(s32, s32);                        /* extern */
void *Item_LookupBaseData(s32);                           /* extern */
extern s32 g_MenuBattleEquipMode;

void Battle_UseItem(s32 arg0) {
    s32 sp10;
    s32 temp_s0;
    void *temp_v0;
    void *temp_v0_2;

    temp_v0 = Item_LookupBaseData(arg0 + 0xEB);
    if (g_MenuBattleEquipMode != 0) {
        temp_s0 = Inv_GetSlotHighlightState(arg0, BattleCmd_GetRemainingAmmo(0));
        temp_v0_2 = BattleCmd_AllocSlot();
        M2C_FIELD(temp_v0_2, s32 *, 0) = 1;
        M2C_FIELD(temp_v0_2, s32 *, 4) = arg0;
        M2C_FIELD(temp_v0_2, s32 *, 8) = temp_s0;
        sp10 = arg0;
        Inv_SetActiveList(1, &sp10);
        return;
    }
    if (M2C_FIELD(temp_v0, u8 *, 0xE) == 1) {
        BattleCmd_ChangeWeaponAndSync(arg0);
    }
}
