#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"
#include "pe1/text.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)



extern s32 g_InvCategoryBaseItemId;
extern u8 g_EquipItemDataTable[];

void *Inv_LookupActiveListDisplayData(s32 index);

void *Inv_LookupActiveListDisplayData(s32 index) {
    s16 itemId;
    u32 lookupIndex;
    ItemDataRecord *entry;
    void *displayData;

    itemId = g_InvItemPtr[index];
    displayData = NULL;
    if ((itemId - 0x100) < 0x80U) {
        entry = &((ItemDataRecord *)g_EquipItemDataTable)[itemId];
        if (entry->flags & 0x10) {
            displayData = g_EquipItemDataTable + 0x31F8;
            if (entry->kind == 9) {
                displayData = g_EquipItemDataTable + 0x3208;
            }
        } else {
            lookupIndex = entry->itemId - 1;
            displayData = Str_LookupTable8(lookupIndex);
        }
    } else {
        lookupIndex = itemId - 1;
        if (lookupIndex >= 0xFFU) {
            if ((itemId - 0x200) < 9U) {
                lookupIndex = (g_InvCategoryBaseItemId + itemId) - 0x201;
                displayData = Str_LookupTable8(lookupIndex);
            }
        } else {
            displayData = Str_LookupTable8(lookupIndex);
        }
    }
    return displayData;
}
