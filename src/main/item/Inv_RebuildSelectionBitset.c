#include "pe1/inventory_slots.h"
extern short D_800C1F80[];
typedef struct InventorySlotRange {
    s16 start;
    s16 stop;
} InventorySlotRange;
extern InventorySlotRange D_800923D8[];
extern InventorySlotRange D_800923F8[];
extern int D_800A77F0[];

void Inv_RebuildSelectionBitset(void) {
    ItemDataRecord *entry;
    register ItemDataRecord *limit_tmp asm("$3");
    register ItemDataRecord *end asm("$5");
    short *list;
    InventorySlotRange *range;
    short *range_stop_ptr;
    InventorySlotRange *range_end;
    int item_id;
    int start;
    int stop;
    int *table;
    int *table_base;

    entry = D_800C0E20.equipment;
    limit_tmp = entry + 0x80;
    if (entry < limit_tmp) {
        end = limit_tmp;
        do {
            entry->flags &= 0xF7;
            entry++;
        } while (entry < end);
    }

    start = 0;
    list = D_800C1F80;
    while (start < 0x52) {
        item_id = *list - 0x100;
        if ((unsigned int)item_id < 0x80) {
            D_800C0E20.equipment[item_id].flags |= 8;
        }
        start++;
        list++;
    }

    range = D_800923D8;
    if (range < D_800923D8 + 8) {
        table_base = D_800A77F0;
        range_stop_ptr = &range->stop;
        do {
            start = range->start;
            stop = *range_stop_ptr;
            if (stop >= start) {
                table = (int *)((start << 2) + (int)table_base);
                do {
                    item_id = *table - 0x100;
                    if ((unsigned int)item_id < 0x80) {
                        D_800C0E20.equipment[item_id].flags |= 8;
                    }
                    stop = *range_stop_ptr;
                    start++;
                    table++;
                } while (stop >= start);
            }
            range++;
            asm volatile("" : "=r"(range_end) : "0"(D_800923F8));
            range_stop_ptr += 2;
        } while (range < range_end);
    }

    entry = D_800C0E20.equipment;
    limit_tmp = entry + 0x80;
    if (entry < limit_tmp) {
        end = limit_tmp;
        do {
            if ((entry->flags & 0x18) == 0) {
                entry->pad_00[0] = 0;
            }
            entry->flags &= 0xF7;
            entry++;
        } while (entry < end);
    }
}
