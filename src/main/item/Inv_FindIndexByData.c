#include "common.h"
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */


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
                item_id = g_InvItemPtr[index];
                saved_item_id = item_id;
                if ((unsigned int)(item_id - 0x100) < 0x80) {
                    data = &D_800C0E20.equipment[item_id - 0x100];
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

    {
        int result;
        result = -1;
        if (index < g_InvSlotLimit) {
            result = index;
        }
        return result;
    }
}
