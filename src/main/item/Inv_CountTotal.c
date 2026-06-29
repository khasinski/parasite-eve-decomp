/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern short g_AyaInventoryItems[];
int g_InvItemPtr;
int g_InvSlotLimit;
int g_InvSelectionBits;
extern int g_AyaItemSelectionBits[];
int g_InvSelectionBitWords;

int Inv_GetAyaSlotLimit(void);

int Inv_CountTotal(void) {
    short *item;
    short *end;
    int count;

    g_InvItemPtr = (int)g_AyaInventoryItems;
    count = 0;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = (int)g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;

    item = (short *)g_InvItemPtr;
    end = item + g_InvSlotLimit;
    while (item < end) {
        count += *item != 0;
        item++;
    }

    return count;
}
