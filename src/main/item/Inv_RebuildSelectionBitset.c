extern unsigned char D_800C0EAC[];
extern unsigned char D_800C0EB1[];
extern short D_800C1F80[];
extern short D_800923D8[];
extern short D_800923F8[];
extern int D_800A77F0[];

void Inv_RebuildSelectionBitset(void) {
    unsigned char *entry;
    register unsigned char *limit_tmp asm("$3");
    register unsigned char *end asm("$5");
    short *list;
    short *range;
    short *range_stop_ptr;
    short *range_end;
    int item_id;
    int start;
    int stop;
    int *table;
    int *table_base;

    entry = D_800C0EAC;
    limit_tmp = entry + 0x1000;
    if (entry < limit_tmp) {
        end = limit_tmp;
        do {
            entry[5] &= 0xF7;
            entry += 0x20;
        } while (entry < end);
    }

    start = 0;
    list = D_800C1F80;
    while (start < 0x52) {
        item_id = *list - 0x100;
        if ((unsigned int)item_id < 0x80) {
            D_800C0EB1[item_id << 5] |= 8;
        }
        start++;
        list++;
    }

    range = D_800923D8;
    if (range < D_800923D8 + 0x10) {
        table_base = D_800A77F0;
        range_stop_ptr = range + 1;
        do {
            start = *range;
            stop = *range_stop_ptr;
            if (stop >= start) {
                table = (int *)((start << 2) + (int)table_base);
                do {
                    item_id = *table - 0x100;
                    if ((unsigned int)item_id < 0x80) {
                        D_800C0EB1[item_id << 5] |= 8;
                    }
                    stop = *range_stop_ptr;
                    start++;
                    table++;
                } while (stop >= start);
            }
            range += 2;
            asm volatile("" : "=r"(range_end) : "0"(D_800923F8));
            range_stop_ptr += 2;
        } while (range < range_end);
    }

    entry = D_800C0EAC;
    limit_tmp = entry + 0x1000;
    if (entry < limit_tmp) {
        end = limit_tmp;
        do {
            if ((entry[5] & 0x18) == 0) {
                entry[0] = 0;
            }
            entry[5] &= 0xF7;
            entry += 0x20;
        } while (entry < end);
    }
}
