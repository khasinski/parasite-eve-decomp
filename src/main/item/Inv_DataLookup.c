/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory.h"
#include "common.h"
#include "pe1/inventory_slots.h"

/* Inventory item-ID and active-slot lookup helpers. */

ItemDataRecord *Inv_LookupData(int arg0) {
    if ((arg0 - 0x100U) < 0x80U) {
        return &D_800C0E20.equipment[arg0 - 0x100];
    }

    if ((arg0 - 1U) < 0xFFU) {
        return Item_LookupBaseData(arg0 - 1);
    }

    if ((arg0 - 0x200U) >= 9U) {
        return 0;
    }

    return (ItemDataRecord *)&g_KeyItemDataTable[arg0 << 5];
}

int g_InvSlotLimit;
s16 *g_InvItemPtr;

ItemDataRecord *Inv_LookupActiveListData(int index) {
    int value;
    int saved_value;
    ItemDataRecord *entry;

    if (index >= 0 && index < g_InvSlotLimit) {
        value = g_InvItemPtr[index];
        saved_value = value;
        if ((unsigned int)(value - 0x100) < 0x80) {
            entry = &D_800C0E20.equipment[value - 0x100];
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

    return entry;
}

int Inv_FindIndexByData(void *needle) {
    int limit;
    int index;
    int item_id;
    int offset;
    void *data;
    int saved_item_id;
    int stack_pad[2];

    limit = g_InvSlotLimit;
    index = 0;
    if (limit > 0) {
        do {
            if (index < 0 || index >= limit) {
                data = 0;
            } else {
                item_id = g_InvItemPtr[index];
                saved_item_id = (s16)item_id;
                if ((unsigned int)(item_id - 0x100) < 0x80) {
                    data = &D_800C0E20.equipment[item_id - 0x100];
                } else {
                    offset = item_id - 1;
                    if ((unsigned int)offset < 0xFF) {
                        data = Item_LookupBaseData(offset);
                    } else if ((unsigned int)(saved_item_id - 0x200) < 9) {
                        int shifted;

                        shifted = saved_item_id << 5;
                        data = g_KeyItemDataTable + shifted;
                    } else {
                        data = 0;
                    }
                }
            }

            if (data == needle) {
                break;
            }
            limit = g_InvSlotLimit;
            index++;
        } while (index < limit);
    }

    {
        int result;
        result = -1;
        if (index < g_InvSlotLimit) {
            result = index;
        }
        return result;
    }
}

short Inv_GetActiveListItem(int index) {
    return g_InvItemPtr[index];
}
