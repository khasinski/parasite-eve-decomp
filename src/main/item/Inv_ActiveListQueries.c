/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory_slots.h"

/* Contiguous inventory counts, removal-by-ID, and active-item kind lookup. */
s16 *g_InvItemPtr;
int g_InvSlotLimit;
u32 *g_InvSelectionBits;
int g_InvSelectionBitWords;

extern short g_WayneStorageItems[];



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

int Inv_FindItemById(int arg0) {
    short *item;
    short *end;
    int result;

    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;

    item = g_InvItemPtr;
    end = item + g_InvSlotLimit;

    if (item < end) {
        while (*item != arg0) {
            item++;
            if (item >= end) {
                break;
            }
        }
        if (item < g_InvItemPtr + g_InvSlotLimit) {
            result = item - g_InvItemPtr;
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

int Inv_GetActiveListItemType(int index)
{
    int value;
    int saved_value;
    ItemDataRecord *entry;

    if (index >= 0 && index < g_InvSlotLimit) {
        value = g_InvItemPtr[index];
        saved_value = value;
        if ((unsigned int)(value - 0x100) < 0x80) {
            entry = (ItemDataRecord *)(g_EquipItemDataTable + (value << 5));
        } else if ((unsigned int)(value - 1) < 0xFF) {
            entry = Item_LookupBaseData(value - 1);
        } else if ((unsigned int)(saved_value - 0x200) < 9) {
            entry = (ItemDataRecord *)(g_KeyItemDataTable + (saved_value << 5));
        } else {
            entry = 0;
        }
    } else {
        entry = 0;
    }

    if (entry == 0) {
        return 0;
    }
    return entry->kind;
}
