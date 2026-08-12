#include "pe1/inventory_slots.h"

void Inv_CompactActiveListSlots(void) {
    short *slot;
    short *end;
    int emptyIndex;
    int capacity;
    int sourceIndex;

    while (g_InvPendingCompactCount > 0) {
        slot = g_InvItemPtr;
        end = g_InvItemPtr + g_InvSlotLimit;
        while (slot < end && *slot != 0) {
            slot++;
        }
        if (slot < g_InvItemPtr + g_InvSlotLimit) {
            emptyIndex = slot - g_InvItemPtr;
        } else {
            emptyIndex = -1;
        }

        if (emptyIndex >= 0) {
            if (g_InvBaseCapacity + Inv_GetBonusSlotCount() < 51) {
                capacity = g_InvBaseCapacity + Inv_GetBonusSlotCount();
            } else {
                capacity = 50;
            }
            sourceIndex = capacity - g_InvPendingCompactCount;
            g_InvItemPtr[emptyIndex] = g_InvItemPtr[sourceIndex];
            g_InvItemPtr[sourceIndex] = 0;
            if (g_InvTrackedSlots[0] == sourceIndex) {
                g_InvTrackedSlots[0] = emptyIndex;
            } else if (g_InvTrackedSlots[2] == sourceIndex) {
                g_InvTrackedSlots[2] = emptyIndex;
            }
        }
        g_InvPendingCompactCount--;
    }
}
