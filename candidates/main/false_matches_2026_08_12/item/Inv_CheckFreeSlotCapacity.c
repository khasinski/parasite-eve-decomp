#include "pe1/inventory_slots.h"

int Inv_CheckFreeSlotCapacity(int requestedSlots) {
    short *slot;
    short *end;
    int capacity;
    int usedSlots;

    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;

    if (g_InvBaseCapacity + Inv_GetBonusSlotCount() < 51) {
        capacity = g_InvBaseCapacity + Inv_GetBonusSlotCount();
    } else {
        capacity = 50;
    }

    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    usedSlots = 0;
    slot = g_InvItemPtr;
    end = g_InvItemPtr + g_InvSlotLimit;
    while (slot < end) {
        usedSlots += *slot > 0;
        slot++;
    }

    g_InvPendingCompactCount = requestedSlots;
    return requestedSlots <= capacity - usedSlots;
}
