#include "common.h"
#include "pe1/inventory_slots.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */


void Inv_BuildFilteredPackedList(int mask) {
    int limit;
    int index;
    int item_id;
    int offset;
    ItemDataRecord *data;
    int type;
    s16 *out;
    int saved_item_id;
    int stack_pad[2];

    limit = g_InvSlotLimit;
    out = D_800A1D9C;
    index = 0;
    if (limit > 0) {
        do {
            if (index < 0 || index >= limit) {
                data = 0;
            } else {
                item_id = g_InvItemPtr[index];
                saved_item_id = (s16)item_id;
                if ((unsigned int)(item_id - 0x100) < 0x80) {
                    data = (ItemDataRecord *)(g_EquipItemDataTable +
                                              (item_id << 5));
                } else {
                    offset = item_id - 1;
                    if ((unsigned int)offset < 0xFF) {
                        data = Item_LookupBaseData(offset);
                    } else if ((unsigned int)(saved_item_id - 0x200) < 9) {
                        int shifted;

                        shifted = saved_item_id << 5;
                        data = (ItemDataRecord *)(g_KeyItemDataTable + shifted);
                    } else {
                        data = 0;
                    }
                }
            }

            if (data != 0) {
                type = data->kind;
            } else {
                type = 0;
            }
            if (((mask >> type) & 1) != 0) {
                *out++ = index;
            }

            limit = g_InvSlotLimit;
            index++;
        } while (index < limit);
    }

    D_8009D068 = 0;
    D_8009D040 = (out - D_800A1D9C);

}

void Inv_BuildFilteredPackedListExcluding(int mask, int excluded) {
    int limit;
    int index;
    int item_id;
    int offset;
    ItemDataRecord *lookup;
    ItemDataRecord *data;
    int type;
    s16 *out;
    int saved_item_id;
    int stack_pad[2];

    limit = g_InvSlotLimit;
    out = D_800A1D9C;
    index = 0;
    if (limit > 0) {
        do {
            if (index < 0 || index >= limit) {
                goto invalid_index;
            } else {
                item_id = g_InvItemPtr[index];
                saved_item_id = item_id;
                if ((unsigned int)(item_id - 0x100) < 0x80) {
                    lookup = (ItemDataRecord *)(g_EquipItemDataTable +
                                               (item_id << 5));
                } else {
                    offset = item_id - 1;
                    if ((unsigned int)offset < 0xFF) {
                        lookup = Item_LookupBaseData(offset);
                    } else if ((unsigned int)(saved_item_id - 0x200) < 9) {
                        int shifted;
                        shifted = saved_item_id << 5;
                        lookup = (ItemDataRecord *)(g_KeyItemDataTable + shifted);
                    } else {
                        lookup = 0;
                    }
                }
                data = lookup;
                goto filter;
            }

invalid_index:
            data = 0;
filter:
            if ((data != 0) &&
                    (((mask >> data->kind) & 1) != 0) &&
                    (data->tailCount != 0) &&
                    (index != excluded)) {
                *out++ = index;
            }

            limit = g_InvSlotLimit;
            index++;
        } while (index < limit);
    }

    D_8009D068 = 0;
    D_8009D040 = (out - D_800A1D9C);

}
