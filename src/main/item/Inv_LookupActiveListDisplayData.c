#include "pe1/inventory.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)


void *Str_LookupTable8();

extern s32 g_InvCategoryBaseItemId;
extern s32 g_InvItemPtr;
extern u8 g_EquipItemDataTable[];

void *Inv_LookupActiveListDisplayData(s32 index);

void *Inv_LookupActiveListDisplayData(s32 index) {
    s16 itemId;
    register u32 lookupIndex asm("$4");
    ItemDataRecord *entry;
    void *displayData;

    itemId = ((s16 *)g_InvItemPtr)[index];
    displayData = NULL;
    if ((itemId - 0x100) < 0x80U) {
        entry = (itemId << 5) + g_EquipItemDataTable;
        if (entry->flags & 0x10) {
            displayData = g_EquipItemDataTable + 0x31F8;
            if (entry->kind == 9) {
                displayData = g_EquipItemDataTable + 0x3208;
            }
        } else {
            lookupIndex = entry->itemId - 1;
            goto lookup_base_data;
        }
    } else {
        lookupIndex = itemId - 1;
        if (lookupIndex >= 0xFFU) {
            if ((itemId - 0x200) < 9U) {
                lookupIndex = (g_InvCategoryBaseItemId + itemId) - 0x201;
                goto lookup_base_data;
            }
        } else {
lookup_base_data:
            displayData = Str_LookupTable8(lookupIndex);
        }
    }
    return displayData;
}
