/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;

extern int g_InvSlotLimit __asm__("D_8009D050");
extern int g_InvItemPtr __asm__("D_8009D048");
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");

void *Item_LookupBaseData(unsigned int index);

int Inv_FindIndexByData(void *needle) {
    int limit;
    int index;
    int item_id;
    int offset;
    void *data;
    register int saved_item_id asm("$5");
    int stack_pad[2];

    limit = g_InvSlotLimit;
    index = 0;
    if (limit > 0) {
        do {
            if (index < 0 || index >= limit) {
                data = 0;
            } else {
                item_id = ((s16 *)g_InvItemPtr)[index];
                saved_item_id = item_id;
                if ((unsigned int)(item_id - 0x100) < 0x80) {
                    data = g_EquipItemDataTable + (item_id << 5);
                } else {
                    offset = item_id - 1;
                    if ((unsigned int)offset < 0xFF) {
                        data = Item_LookupBaseData(offset);
                    } else if ((unsigned int)(saved_item_id - 0x200) < 9) {
                        register int shifted asm("$3");

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

    asm volatile("" : "=m"(stack_pad));
    {
        register int result asm("$3");

        result = -1;
        if (index < g_InvSlotLimit) {
            result = index;
        }
        return result;
    }
}
