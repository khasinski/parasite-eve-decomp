#include "common.h"
#include "pe1/inventory_slots.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */


int Inv_GetActiveListItemType(int index)
{
    int value;
    int saved_value;
    ItemDataRecord *entry;

    if (index >= 0 && index < g_InvSlotLimit) {
        value = g_InvItemPtr[index];
        saved_value = value;
        if ((unsigned int)(value - 0x100) < 0x80) {
            entry = (ItemDataRecord *)(g_EquipItemDataTable + (value << 5));
        } else if ((unsigned int)(value - 1) < 0xFF) {
            entry = Item_LookupBaseData(value - 1);
        } else if ((unsigned int)(saved_value - 0x200) < 9) {
            entry = (ItemDataRecord *)(g_KeyItemDataTable + (saved_value << 5));
        } else {
            entry = 0;
        }
    } else {
        entry = 0;
    }

    if (entry == 0) {
        return 0;
    }
    return entry->kind;
}
