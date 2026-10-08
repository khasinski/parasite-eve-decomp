/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"

int g_InvSlotLimit;

int Inv_GetActiveListItemType(int arg0);

extern short g_WayneStorageItems[];

s16 *g_InvItemPtr;
int g_InvSlotLimit;
u32 *g_InvSelectionBits;
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
            if (Item_LookupBaseData(itemId - 1)->kind == arg0) {
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

    g_InvItemPtr = g_AyaInventoryItems;
    count = 0;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;

    item = g_InvItemPtr;
    end = item + g_InvSlotLimit;
    while (item < end) {
        count += *item != 0;
        item++;
    }

    return count;
}
