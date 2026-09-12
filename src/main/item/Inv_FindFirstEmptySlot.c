#include "pe1/inventory_slots.h"

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

/* Find a run of empty slots, after selecting Aya's inventory. The two
 * register bindings keep the scan limits in their retail registers and
 * are recorded in debt. No barriers or instruction assembly are needed. */
int Inv_FindFirstEmptySlot(int count) {
    short *p, *base;
    register short *end asm("$3");
    int n;

    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    p = g_InvItemPtr;
    end = p + g_InvSlotLimit - count;
    if (p < end + 1) {
        base = p;
        {
            register short *limit asm("$6") = end;
            do {
                register short *bound = limit + 1;
                while (1) {
                    if (!*p) break;
                    p++;
                    if (p >= bound) break;
                }
                if (p < limit + 1) {
                    for (n = 1; n < count && p[n] == 0; n++) {
                    }
                    if (n >= count) return p - base;
                }
                p++;
            } while (p < limit + 1);
        }
    }
    return -1;
}
