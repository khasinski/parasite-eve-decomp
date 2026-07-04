/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_InvItemPtr;
int g_InvActiveListOverride;
int g_InvSlotLimit;
int g_InvOverrideSlotLimit;
int g_InvSelectionBits;
int g_InvSelectionBitWords;
extern int g_AyaItemSelectionBits[];
extern short g_AyaInventoryItems[];
extern int g_InvStorageSelectionBits[];

int Inv_GetAyaSlotLimit(void);

void Inv_SelectActiveList(int useOverride) {
    if (useOverride != 0 && g_InvActiveListOverride != 0) {
        g_InvItemPtr = g_InvActiveListOverride;
        g_InvSelectionBits = (int)g_InvStorageSelectionBits;
        g_InvSelectionBitWords = 4;
        g_InvSlotLimit = g_InvOverrideSlotLimit;
    } else {
        g_InvItemPtr = (int)g_AyaInventoryItems;
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
        g_InvSelectionBits = (int)g_AyaItemSelectionBits;
        g_InvSelectionBitWords = 2;
    }
}
