#include "pe1/inventory_slots.h"

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

/* Separate views prevent the base-capacity address from being retained
 * across calls, as in Inv_CheckFreeSlotCapacity. Both aliases are debt. */
extern u8 g_InvBaseCapacityForLimit[] asm("D_800C0E0C");
extern u8 g_InvBaseCapacityForResult[] asm("D_800C0E0C");

void Inv_CompactActiveListSlots(void) {
    int index, limit, from;
    register int slot;
    short *p, *end;

    while (g_InvPendingCompactCount > 0) {
        p = g_InvItemPtr;
        end = p + g_InvSlotLimit;
        if (p < end) {
            do {
                if (!*p) break;
                p++;
            } while (p < end);
            if (p < g_InvItemPtr + g_InvSlotLimit) {
                slot = p - g_InvItemPtr;
            } else {
                slot = -1;
            }
        } else {
            slot = -1;
        }
        index = slot;
        if (index >= 0) {
            if (g_InvBaseCapacityForLimit[0] + Inv_GetBonusSlotCount() < 51) {
                limit = g_InvBaseCapacityForResult[0] + Inv_GetBonusSlotCount();
            } else {
                limit = 50;
            }
            from = limit - g_InvPendingCompactCount;
            g_InvItemPtr[index] = g_InvItemPtr[from];
            g_InvItemPtr[from] = 0;
            if (g_InvTrackedSlots[0] == from) {
                g_InvTrackedSlots[0] = index;
            } else if (g_InvTrackedSlots[2] == from) {
                g_InvTrackedSlots[2] = index;
            }
        }
        g_InvPendingCompactCount--;
    }
}
