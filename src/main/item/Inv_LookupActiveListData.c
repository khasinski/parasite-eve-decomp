#include "common.h"
#include "pe1/inventory.h"
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_InvSlotLimit;
int g_InvItemPtr;
extern u8 g_EquipItemDataTable[];
extern u8 g_KeyItemDataTable[];

ItemDataRecord *Inv_LookupActiveListData(int index) {
    int value;
    int saved_value;
    u8 *entry;

    if (index >= 0 && index < g_InvSlotLimit) {
        value = ((s16 *)g_InvItemPtr)[index];
        saved_value = value;
        if ((unsigned int)(value - 0x100) < 0x80) {
            entry = g_EquipItemDataTable + (value << 5);
        } else if ((unsigned int)(value - 1) < 0xFF) {
            entry = (u8 *)Item_LookupBaseData(value - 1);
        } else if ((unsigned int)(saved_value - 0x200) < 9) {
            entry = g_KeyItemDataTable + (saved_value << 5);
        } else {
            entry = 0;
        }
    } else {
        entry = 0;
    }

    return (ItemDataRecord *)entry;
}
