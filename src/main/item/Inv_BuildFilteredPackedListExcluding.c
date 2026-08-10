#include "common.h"
#include "pe1/inventory.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_InvSlotLimit __asm__("D_8009D050");
extern int g_InvItemPtr __asm__("D_8009D048");
extern int g_InvActiveListCount __asm__("D_8009D040");
extern int D_8009D068;
extern s16 D_800A1D9C[];
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");

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
                item_id = ((s16 *)g_InvItemPtr)[index];
                saved_item_id = item_id;
                if ((unsigned int)(item_id - 0x100) < 0x80) {
                    lookup = (ItemDataRecord *)(g_EquipItemDataTable +
                                               (item_id << 5));
                    goto have_lookup;
                } else {
                    offset = item_id - 1;
                    if ((unsigned int)offset < 0xFF) {
                        lookup = Item_LookupBaseData(offset);
                        goto have_lookup;
                    } else if ((unsigned int)(saved_item_id - 0x200) < 9) {
                        int shifted;
                        shifted = saved_item_id << 5;
                        lookup = (ItemDataRecord *)(g_KeyItemDataTable + shifted);
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
    g_InvActiveListCount = (out - D_800A1D9C);

}
