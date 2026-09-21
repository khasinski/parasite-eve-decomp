/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory_slots.h"
static inline ItemDataRecord *LookupItem(int value) {
    int saved = value;
    ItemDataRecord *result;
    if ((unsigned)(value - 0x100) < 0x80) {
        result = &D_800C0E20.equipment[value - 0x100];
    } else {
        if ((unsigned)(value - 1) < 0xFF) {
            result = Item_LookupBaseData(value - 1);
        } else if ((unsigned)(saved - 0x200) < 9) {
            result = &D_800A1E64[saved - 512];
        } else {
            result = 0;
        }
    }
    return result;
}

static inline int FindFreeActiveSlot(void) {
    s16 *p = D_8009D048, *end = p + D_8009D050;

    if (p < end) {
        do {
            if (!*p)
                break;
            p++;
        } while (p < end);
        if (p < D_8009D048 + D_8009D050)
            return p - D_8009D048;
    }
    return -1;
}


/* Transfer pending IDs, then rebuild the selectable-item mask. */
void Inv_TransferItemAlt(void) {
    int i = 0;
    D_8009D048 = D_800C0E48;
    D_8009D050 = Inv_GetAyaSlotLimit();
    D_8009D058 = D_8009D05C;
    D_8009D064 = 2;
    for (; i < D_8009D078; i++) {
        int id = D_800A1FD4[i];
        if (LookupItem(id)->kind >= 16 && LookupItem(id)->kind < 19) {
            if (!Inv_CanAddActiveListItemToAya(i))
                D_800A1FD4[i] = 0;
        } else if (id < 512) {
            int slot = FindFreeActiveSlot();
            if (slot >= 0) {
                D_8009D048[slot] = id;
                D_800A1FD4[i] = 0;
            }
        }
    }
    Inv_RebuildSelectableMask();
}
