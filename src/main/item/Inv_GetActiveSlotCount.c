/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory.h"
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

/* Historical name: checks free capacity against the armor-derived reserve.
 * Also selects Aya's inventory and optionally returns the required reserve. */
int Inv_GetActiveSlotCount(int *out) {
    ItemDataRecord *armor;
    int reserve = 0;
    int used;
    int capacity, i, enough, occupied;
    int capacityBeforeScan, bonusSlots;
    s16 *slot, *end;

    armor = LookupTrackedItem(D_800C0E20.tracked[2]);
    if (armor != 0) {
        for (i = 0; i < armor->tailCount; i++) {
            reserve = (armor->tailData[i] & 0x1F) - 8;
            if ((unsigned)reserve < 3) break;
        }
        if (i < armor->tailCount) reserve = 1 << reserve;
        else reserve = 0;
    }
    bonusSlots = Inv_GetBonusSlotCount();
    if (g_InvBaseCapacityForLimit[0] + bonusSlots < 51) {
        used = 0;
        bonusSlots = Inv_GetBonusSlotCount();
        capacityBeforeScan = g_InvBaseCapacityForResult[0] + bonusSlots;
    } else {
        used = 0;
        capacityBeforeScan = 50;
    }
    D_8009D048 = D_800C0E48;
    capacity = capacityBeforeScan;
    D_8009D050 = Inv_GetAyaSlotLimit();
    D_8009D058 = D_8009D05C;
    D_8009D064 = 2;
    slot = D_8009D048;
    end = slot + D_8009D050;
    while (slot < end) {
        occupied = *slot++;
        occupied = occupied != 0;
        used += occupied;
    }
    enough = capacity - used >= reserve;
    if (out != 0) *out = reserve;
    return enough;
}
