#include "pe1/inventory_slots.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_inventory.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_runtime.h"

int Menu_CheckItemAffordable(int actionId)
{
    int total, remaining, enabled, allowed, category;
    /* Matching debt: preserve the action across queries in $s1. */
    register int action asm("$17") = actionId;
    int bits;
    ItemDataRecord *item;
    remaining = BattleCmd_GetRemainingAmmo(&total);
    item = Item_LookupBaseData(action + 235);
    bits = item->flags >> Menu_GetBattleEquipMode();
    if (action == 5) {
        if (remaining >= total / 3) enabled = bits & 1;
        else enabled = 0;
    } else if (action != 6) {
        if (action != 19) {
            if (remaining >= Inv_GetSlotHighlightState(action, 0)) enabled = bits & 1;
            else enabled = 0;
        } else {
            if (remaining >= total) enabled = bits & 1;
            else enabled = 0;
        }
    } else {
        if (remaining > 0) enabled = bits & 1;
        else enabled = 0;
    }
    /* These actions are unavailable while the global restriction is active. */
    allowed = 0;
    if (D_8009CF3C) {
        if ((unsigned)(action - 6) < 3) goto permission_done;
        if (action == 10 || action == 19) goto permission_done;
    }
    allowed = 1;
permission_done:
    enabled &= allowed;
    if (action == 5 && Battle_GetEnemyEscapeFlag()) enabled = 0;
    if (action == 6) {
        ItemDataRecord *weapon;
        weapon = Inv_LookupActiveListData(g_InvTrackedSlots[0]);
        category = weapon->kind;
        if (category == 8) enabled = 0;
        else {
            if (category && (unsigned)category < 8) {
                category -= 4;
                if (category <= 0) category = 1;
            } else {
                category = weapon->kind < 19 ? 0 : weapon->kind - 18;
            }
            if (category == 1 || category == 3) {
                int ammo = Inv_GetWeaponCategoryAmmoBase(category - 1);
                enabled &= weapon->ammo + ammo > 0 ? 1 : 0;
            } else enabled = 0;
        }
    }
    if (action < 3 || action == 18) {
        if (D_800C0E08[0] >= D_800C0E06[0]) enabled = 0;
    }
    return enabled;
}
