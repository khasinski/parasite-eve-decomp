#include "common.h"
#include "pe1/battle_cmd.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
BattleCmdEntry *BattleCmd_AllocSlot();                    /* extern */
int Inv_SetActiveList(int, s32 *);                       /* extern */
s32 BattleCmd_GetRemainingAmmo(int);                     /* extern */
int BattleCmd_ChangeWeaponAndSync(s32);                  /* extern */
s32 Inv_GetSlotHighlightState(s32, s32);                        /* extern */
void *Item_LookupBaseData(s32);                           /* extern */
extern s32 g_MenuBattleEquipMode;

void Battle_UseItem(s32 arg0) {
    s32 sp10;
    s32 temp_s0;
    void *temp_v0;
    BattleCmdEntry *temp_v0_2;

    temp_v0 = Item_LookupBaseData(arg0 + 0xEB);
    if (g_MenuBattleEquipMode != 0) {
        temp_s0 = Inv_GetSlotHighlightState(arg0, BattleCmd_GetRemainingAmmo(0));
        temp_v0_2 = BattleCmd_AllocSlot();
        temp_v0_2->header.word = 1;
        temp_v0_2->payload.ammo_spend.item_index = arg0;
        temp_v0_2->payload.ammo_spend.amount = temp_s0;
        sp10 = arg0;
        Inv_SetActiveList(1, &sp10);
        return;
    }
    if (((u8 *)temp_v0)[0xE] == 1) {
        BattleCmd_ChangeWeaponAndSync(arg0);
    }
}
