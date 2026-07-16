/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;

extern int g_InvSlotLimit __asm__("D_8009D050");
extern int g_InvItemPtr __asm__("D_8009D048");
extern int g_InvActiveListCount __asm__("D_8009D040");
extern int D_8009D068;
extern s16 D_800A1D9C[];
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");

void *Item_LookupBaseData(unsigned int index);

void Inv_BuildFilteredPackedListExcluding(int mask, int excluded) {
    int limit;
    int index;
    int item_id;
    int offset;
    void *lookup;
    register void *data asm("$3");
    int type;
    s16 *out;
    register int saved_item_id asm("$5");
    int stack_pad[2];

    limit = g_InvSlotLimit;
    out = D_800A1D9C;
    index = 0;
    if (limit > 0) {
        do {
            if (index < 0 || index >= limit) {
                goto invalid_index;
            } else {
                item_id = ((s16 *)g_InvItemPtr)[index];
                saved_item_id = item_id;
                if ((unsigned int)(item_id - 0x100) < 0x80) {
                    lookup = g_EquipItemDataTable + (item_id << 5);
                    goto have_lookup;
                } else {
                    offset = item_id - 1;
                    if ((unsigned int)offset < 0xFF) {
                        lookup = Item_LookupBaseData(offset);
                        goto have_lookup;
                    } else if ((unsigned int)(saved_item_id - 0x200) < 9) {
                        register int shifted asm("$3");

                        shifted = saved_item_id << 5;
                        lookup = g_KeyItemDataTable + shifted;
                        goto have_lookup;
                    } else {
                        lookup = 0;
                        goto have_lookup;
                    }
                }
            }

have_lookup:
            data = lookup;
            goto filter;
invalid_index:
            data = 0;
filter:
            if ((data != 0) &&
                    (((mask >> ((u8 *)data)[6]) & 1) != 0) &&
                    (((u8 *)data)[0x14] != 0) &&
                    (index != excluded)) {
                *out++ = index;
            }

            limit = g_InvSlotLimit;
            index++;
        } while (index < limit);
    }

    D_8009D068 = 0;
    g_InvActiveListCount = (out - D_800A1D9C);

}
