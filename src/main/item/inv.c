/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

int g_InvActiveListOverride;
int g_InvOverrideSlotLimit;

extern unsigned short g_AyaInventoryItems[];
extern int g_InvActiveListOverride;
extern int g_InvOverrideSlotLimit;
extern unsigned int g_InvItemPtr;
extern int g_InvSlotLimit;
extern unsigned int g_InvSelectionBits;
extern int g_InvSelectionBitWords;
extern int g_AyaItemSelectionBits[];

int Inv_GetAyaSlotLimit(void);

void Inv_SetActiveListOverride(int items, int count) {
    g_InvActiveListOverride = items;
    g_InvOverrideSlotLimit = count;
}

void Inv_ResetActiveList(void) {
    g_InvActiveListOverride = 0;
    g_InvOverrideSlotLimit = 0;
    g_InvItemPtr = (unsigned int)g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = (unsigned int)&g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;
}
