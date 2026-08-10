#include "common.h"
#include "pe1/battle_cmd.h"
#include "pe1/inventory.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

extern BattleCmdEntry *D_8009D014;
extern BattleCmdEntry D_800A1AA0[];
extern s16 D_800C0E48[];
extern s8 D_800C0E20[];
extern s8 D_800C0E22[];

int Inv_CheckSlotUsable(int data);
int Inv_FindIndexByData(int data);

void BattleCmd_UndoPending(void) {
    BattleCmdEntry *entry;
    BattleCmdEntry *entry_tmp;
    BattleCmdEntry *top;
    int opcode;
    s16 *slot;
    int data;
    int index;

    top = D_8009D014;
    if (D_800A1AA0 < top) {
        entry_tmp = top - 1;
        opcode = (top - 1)->header.word;
        D_8009D014 = entry_tmp;
        asm volatile("" : "=r"(entry) : "0"(entry_tmp));

        switch (opcode) {
        case 0:
            slot = &D_800C0E48[entry->payload.inventory_restore.slot_index];
            if (*slot == 0) {
                *slot = entry->payload.inventory_restore.item_id;
            } else {
                Inv_CheckSlotUsable(entry->payload.inventory_restore.item_id);
            }
            break;

        case 1:
            break;

        case 2:
            D_800C0E20[0] =
                Inv_FindIndexByData(entry->payload.equip_restore.item_data);
            break;

        case 3:
            data = entry->payload.equip_restore.item_data;
            index = -1;
            if (data != 0) {
                index = Inv_FindIndexByData(data);
            }
            D_800C0E22[0] = index;
            break;

        case 4:
            ((ItemDataRecord *)entry->payload.ammo_restore.item_data0)->ammo =
                entry->payload.ammo_restore.ammo0;
            ((ItemDataRecord *)entry->payload.ammo_restore.item_data1)->ammo =
                entry->payload.ammo_restore.ammo1;
            ((ItemDataRecord *)entry->payload.ammo_restore.item_data2)->ammo =
                entry->payload.ammo_restore.ammo2;
            break;
        }
    }
}
