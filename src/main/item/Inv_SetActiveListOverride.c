/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_InvActiveListOverride;
int g_InvOverrideSlotLimit;

void Inv_SetActiveListOverride(int items, int count) {
    g_InvActiveListOverride = items;
    g_InvOverrideSlotLimit = count;
}
