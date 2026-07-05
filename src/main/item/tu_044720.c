/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

int g_InvSlotLimit;

int Inv_GetActiveListItemType(int arg0);

extern short g_WayneStorageItems[];

void *Item_LookupBaseData(int arg0);

extern short g_AyaInventoryItems[];
int g_InvItemPtr;
int g_InvSlotLimit;
int g_InvSelectionBits;
extern int g_AyaItemSelectionBits[];
int g_InvSelectionBitWords;

int Inv_GetAyaSlotLimit(void);

int Inv_CountByCategory(int arg0) {
    int index;
    int count;

    index = 0;
    count = 0;
    while (index < g_InvSlotLimit) {
        if (Inv_GetActiveListItemType(index) == arg0) {
            count++;
        }
        index++;
    }

    return count;
}

int WayneStorage_CountItemType(int arg0) {
    short *item;
    int index;
    int count;
    int itemId;

    count = 0;
    index = 0;
    item = g_WayneStorageItems;
    do {
        itemId = *item;
        if (itemId != 0) {
            if (((unsigned char *)Item_LookupBaseData(itemId - 1))[6] == arg0) {
                count++;
            }
        }
        index++;
        item++;
    } while (index < 0x64);

    return count;
}

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
