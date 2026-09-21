/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"

static inline ItemDataRecord *LookupTrackedItem(int index) {
    int value, saved;
    ItemDataRecord *result;

    if (index >= 0 && index < D_8009D050) {
        value = D_8009D048[index];
        saved = value;
        if ((unsigned)(value - 0x100) < 0x80) {
            result = &D_800C0E20.equipment[value - 0x100];
        } else {
            if ((unsigned)(value - 1) < 0xFF) {
                return Item_LookupBaseData(value - 1);
            }
            if ((unsigned)(saved - 0x200) < 9) {
                int shifted = saved << 5;
                result = (ItemDataRecord *)(D_8009DE64 + shifted);
            } else {
                result = 0;
            }
        }
        /* Keep the item-ID result merge distinct from an invalid list index. */
        asm("");
        return result;
    }
    return 0;
}

int Inv_IsSlotSelectable(int index) {
    ItemDataRecord *data;
    int selectable, count, i;
    int kind;
    data = LookupTrackedItem(index);
    if (data == 0) {
        return 1;
    }
    selectable = 0;
    if (!(data->flags & ITEM_DATA_FLAG_DISABLED)) {
        if (D_8009D048 != D_800C0E48 || index != D_800C0E20.tracked[0]) {
            selectable = 1;
        }
    }
    if (data != 0 && data->kind == 8) {
        kind = 8;
        count = 0;
        for (i = 0; i < D_8009D050; i++) {
            if (Inv_GetActiveListItemType(i) == kind) {
                count++;
            }
        }
        selectable &= count >= 2;
    }
    return selectable;
}

void Item_SetDisabledFlag(int arg0, int arg1) {
    ItemDataRecord *entry;
    int flags;

    entry = Item_LookupBaseData(arg0 - 1);
    if (entry != 0) {
        flags = entry->flags & (u8)~ITEM_DATA_FLAG_DISABLED;
        entry->flags = flags;
        if (arg1 == 0) {
            flags |= ITEM_DATA_FLAG_DISABLED;
        }
        entry->flags = flags;
    }
}
