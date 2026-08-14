#include "pe1/inventory_slots.h"

/* CC1_FLAGS: -G4 */
/* MASPSX_FLAGS: -G4 */

extern u8 g_InvBaseCapacityForLimit[] asm("D_800C0E0C");
extern u8 g_InvBaseCapacityForResult[] asm("D_800C0E0C");

int Inv_CheckFreeSlotCapacity(int requested_slots) {
    s16 *slot;
    s16 *end;
    int capacity;
    register int capacity_before_scan asm("$3");
    int bonus_slots;
    int used_slots;
    int occupied;

    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;

    bonus_slots = Inv_GetBonusSlotCount();
    if (g_InvBaseCapacityForLimit[0] + bonus_slots < 51) {
        used_slots = 0;
        capacity_before_scan =
            g_InvBaseCapacityForResult[0] + Inv_GetBonusSlotCount();
    } else {
        used_slots = 0;
        capacity_before_scan = 50;
    }

    g_InvItemPtr = g_AyaInventoryItems;
    capacity = capacity_before_scan;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;

    slot = g_InvItemPtr;
    end = slot + g_InvSlotLimit;
    while (slot < end) {
        occupied = *slot++;
        occupied = occupied != 0;
        used_slots += occupied;
    }

    g_InvPendingCompactCount = requested_slots;
    return capacity - used_slots >= requested_slots;
}
