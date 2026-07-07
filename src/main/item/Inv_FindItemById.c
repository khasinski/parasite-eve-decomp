/* MASPSX_FLAGS: -G8 --use-comm-section */

extern short g_AyaInventoryItems[];
int g_InvItemPtr;
int g_InvSlotLimit;
int g_InvSelectionBits;
extern int g_AyaItemSelectionBits[];
int g_InvSelectionBitWords;

int Inv_GetAyaSlotLimit(void);
void Inv_RemoveActiveListItem(int arg0);

int Inv_FindItemById(int arg0) {
    short *item;
    short *end;
    int result;

    g_InvItemPtr = (int)g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = (int)g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;

    item = (short *)g_InvItemPtr;
    end = item + g_InvSlotLimit;

    if (item < end) {
        while (*item != arg0) {
            item++;
            if (item >= end) {
                break;
            }
        }
        if (item < (short *)(g_InvItemPtr + g_InvSlotLimit * 2)) {
            result = item - (short *)g_InvItemPtr;
        } else {
            result = -1;
        }
    } else {
        result = -1;
    }

    if (result >= 0) {
        Inv_RemoveActiveListItem(result);
        return 0;
    }
    return result;
}
