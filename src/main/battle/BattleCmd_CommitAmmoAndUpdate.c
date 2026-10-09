#include "common.h"
#include "pe1/battle_runtime.h"
#include "pe1/aya.h"
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"

int Inv_IsActiveListOverrideSelected(void);
void Inv_SelectActiveList(int mode);
#include "pe1/weapon_list_output.h"

int BattleCmd_CommitAmmoAndUpdate(void *out) {
    Combatant *current;
    ItemDataRecord *entry;
    int saved;
    int result;

    if (D_8009D254 != 0) {
        current = D_8009D254->core;
        if (current != 0) {
            D_800C0E00.current_hp = current->curHP;
            if (current->action != 0) {
                saved = Inv_IsActiveListOverrideSelected();
                Inv_SelectActiveList(0);
                entry = Inv_LookupActiveListData(g_InvTrackedSlots[0]);
                if (entry != 0) {
                    entry->ammo = current->action->attackWord & 0x3FF;
                }
                Inv_SelectActiveList(saved);
            }
        }
    }

    result = Inv_DrawSlotItemIcon();
    Inv_BuildWeaponList(0, out);
    return result;
}

