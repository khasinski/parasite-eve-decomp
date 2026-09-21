/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory_slots.h"

static inline int FindFreeEquipmentRecord(void) {
    int result;
    ItemDataRecord *p = D_800C0E20.equipment;
    ItemDataRecord *end = p + 128;

    if (p < end) {
        do {
            if (!p->pad_00[0])
                break;
            p++;
        } while (p < end);
        if (p < D_800C0E20.equipment + 128) {
            int index = p - D_800C0E20.equipment;
            result = index;
        } else {
            result = -1;
        }
    } else {
        result = -1;
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

ItemDataRecord *Inv_FindSlotByIndex(int id) {
    ItemDataRecord *result = 0;
    int record = FindFreeEquipmentRecord();
    int slot = FindFreeActiveSlot();

    if (record >= 0 && slot >= 0) {
        result = &D_800C0E20.equipment[record];
        *result = *Item_LookupBaseData(id - 1);
        D_8009D048 = D_800C0E20.slots;
        D_8009D050 = Inv_GetAyaSlotLimit();
        D_8009D058 = D_8009D05C;
        D_8009D064 = 2;
        D_8009D048[slot] = record + 256;
    }
    return result;
}
