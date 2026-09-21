/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory.h"
#include "pe1/aya.h"
#include "pe1/battle_cmd.h"
#include "pe1/inventory_slots.h"

static inline ItemDataRecord *LookupTrackedItem(int index) {
    int value, saved;
    ItemDataRecord *result;

    if (index >= 0 && index < D_8009D050) {
        value = D_8009D048[index];
        saved = value;
        if ((unsigned)(value - 0x100) < 0x80) {
            result = &D_800C0E20.equipment[value - 0x100];
        } else {
            if ((unsigned)(value - 1) < 0xFF) {
                return Item_LookupBaseData(value - 1);
            }
            if ((unsigned)(saved - 0x200) < 9) {
                int shifted = saved << 5;
                result = (ItemDataRecord *)(D_8009DE64 + shifted);
            } else {
                result = 0;
            }
        }
        /* Keep the item-ID result merge distinct from an invalid list index. */
        asm("");
        return result;
    }
    return 0;
}

int Inv_GetSlotHighlightState(int spell, int available) {
    int remaining, cost, i;
    ItemDataRecord *armor;
    ParasiteSpellEntry *table;
    if (spell == 6 || spell == 19) {
        return available;
    }
    if (spell == 5) {
        BattleCmd_GetRemainingAmmo(&remaining);
        return remaining / 3;
    }
    table = Aya_GetParasiteSpellUnlockTable();
    cost = table[spell].cost;
    armor = LookupTrackedItem(D_800C0E20.tracked[2]);
    if (armor != 0) {
        for (i = 0; i < armor->tailCount; i++) {
            if (armor->tailData[i] == 0x0E) {
                break;
            }
        }
        if (i < armor->tailCount) {
            cost = cost * 2 / 3;
        }
    }
    return cost;
}
