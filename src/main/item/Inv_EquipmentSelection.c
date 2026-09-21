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
            int shifted = saved << 5;
            result = (ItemDataRecord *)(D_8009DE64 + shifted);
        } else {
            result = 0;
        }
    }
    return result;
}

static inline ItemDataRecord *LookupActiveItem(int index) {
    if (index >= 0 && index < D_8009D050) return LookupItem(D_8009D048[index]);
    return 0;
}

static inline void ClearBits(void) {
    int i;
    for (i = 0; i < D_8009D064; i++) D_8009D058[i] = 0;
}

/* Build the threshold-qualified set; negate its count if a tagged record exists. */
int Inv_RebuildWithBonusSlots(int minSecond, int minThird) {
    int count, i, index;
    ItemDataRecord *item;
    D_8009D048 = D_800C0E48;
    D_8009D050 = Inv_GetAyaSlotLimit();
    D_8009D058 = D_8009D05C;
    D_8009D064 = 2;
    Inv_BuildFilteredPackedList(D_8009D0CC ? 0x1FE : 0x200);
    ClearBits();
    count = 0;
    i = 0;
    while (1) {
        if (i >= D_8009D040) break;
        index = i >= 0 && i < D_8009D040 ? D_800A1D9C[i] : 0;
        item = LookupActiveItem(index);
        if (item->baseStats[0] + item->bonusStats[0] >= D_8009D0D0 &&
            item->baseStats[1] + item->bonusStats[1] >= minSecond &&
            item->baseStats[2] + item->bonusStats[2] >= minThird &&
            item->itemId != 0x93 && item->itemId != 0x61) {
            count++;
            D_8009D058[i >> 5] |= 1u << (i & 31);
        }
        i++;
    }
    if (count) {
        ItemDataRecord *scan;
        if (D_8009D0CC) {
            scan = D_800C0E20.equipment;
            for (; scan < D_800C0E20.equipment + 128 &&
                   !(scan->pad_00[0] && scan->kind < 9 && (scan->flags & 0x10));
                 scan++)
                ;
        } else {
            scan = D_800C0E20.equipment;
            for (; scan < D_800C0E20.equipment + 128 &&
                   !(scan->pad_00[0] && scan->kind == 9 && (scan->flags & 0x10));
                 scan++)
                ;
        }
        if (scan < D_800C0E20.equipment + 128) count = -count;
    }
    D_8009D068 = count != 0;
    return count;
}

/* Historical name: mark filtered records whose tail count is below byte 1. */
int Inv_TransferToStorage(void) {
    int count, i, index;
    ItemDataRecord *item;
    D_8009D048 = D_800C0E48;
    D_8009D050 = Inv_GetAyaSlotLimit();
    D_8009D058 = D_8009D05C;
    D_8009D064 = 2;
    Inv_BuildFilteredPackedList(0x3FE);
    ClearBits();
    count = 0;
    i = 0;
    while (1) {
        if (i >= D_8009D040) break;
        index = i >= 0 && i < D_8009D040 ? D_800A1D9C[i] : 0;
        item = LookupActiveItem(index);
        if (item->tailCount < item->pad_00[1]) {
            count++;
            D_8009D058[i >> 5] |= 1u << (i & 31);
        }
        i++;
    }
    return count;
}
