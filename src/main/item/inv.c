/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/inventory_slots.h"

s16 *g_InvActiveListOverride;
int g_InvOverrideSlotLimit;


void Inv_SetActiveListOverride(s16 *items, int count) {
    g_InvActiveListOverride = items;
    g_InvOverrideSlotLimit = count;
}

void Inv_ResetActiveList(void) {
    g_InvActiveListOverride = 0;
    g_InvOverrideSlotLimit = 0;
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;
}
