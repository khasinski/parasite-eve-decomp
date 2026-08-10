#include "common.h"
#include "pe1/inventory.h"

void Item_SetDisabledFlag(int arg0, int arg1) {
    ItemDataRecord *entry;
    int flags;

    entry = Item_LookupBaseData(arg0 - 1);
    if (entry != 0) {
        flags = entry->flags & 0xBF;
        entry->flags = flags;
        if (arg1 == 0) {
            flags |= 0x40;
        }
        entry->flags = flags;
    }
}
