/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory_slots.h"
#include "pe1/menu_inventory.h"

static inline ItemDataRecord *LookupItem(int value) {
    int saved = value;
    ItemDataRecord *result;
    if ((unsigned)(value - 0x100) < 0x80) {
        result = &D_800C0E20.equipment[value - 0x100];
    } else {
        if ((unsigned)(value - 1) < 0xFF) return Item_LookupBaseData(value - 1);
        if ((unsigned)(saved - 0x200) < 9) {
            int shifted = saved << 5;
            result = (ItemDataRecord *)(D_8009DE64 + shifted);
        } else result = 0;
    }
    return result;
}
static inline int WayneSlot(int index) {
    int result;
    if (index >= 0 && index < D_8009D044) result = D_800A1E00[index];
    else result = 0;
    return result;
}
static inline void SwapSlots(s16 *a, s16 *b) {
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}
static inline void SwapAyaSlots(int firstSlot, int secondSlot) {
    int from = firstSlot;
    int to = secondSlot;
    SwapSlots(&D_8009D048[from], &D_8009D048[to]);
    if (D_800C0E20.tracked[0] == from) D_800C0E20.tracked[0] = to;
    else if (D_800C0E20.tracked[0] == to) D_800C0E20.tracked[0] = from;
    if (D_800C0E20.tracked[2] == from) D_800C0E20.tracked[2] = to;
    else if (D_800C0E20.tracked[2] == to) D_800C0E20.tracked[2] = from;
}
static inline ItemDataRecord *LookupTrackedItem(int index) {
    int value, saved;
    ItemDataRecord *result;

    if (index >= 0 && index < D_8009D050) {
        value = D_8009D048[index];
        saved = value;
        if ((unsigned)(value - 0x100) < 0x80) {
            result = &D_800C0E20.equipment[value - 0x100];
        } else {
            if ((unsigned)(value - 1) < 0xFF) {
                return Item_LookupBaseData(value - 1);
            }
            if ((unsigned)(saved - 0x200) < 9) {
                int shifted = saved << 5;
                result = (ItemDataRecord *)(D_8009DE64 + shifted);
            } else {
                result = 0;
            }
        }
        /* Keep the item-ID result merge distinct from an invalid list index. */
        asm("");
        return result;
    }
    return 0;
}


static inline ItemDataRecord *LookupActiveItem(int index) {
    if (index >= 0 && index < D_8009D050) return LookupItem(D_8009D048[index]);
    return 0;
}

static inline int ItemKind(int slot) {
    ItemDataRecord *item = LookupActiveItem(slot);
    if (item) return item->kind;
    return 0;
}
/* Historical name: collect kind-filtered slots, rebuild selection, then append
 * empty slots. Return the final row count consumed by inventory menus.
 * The C variable shift requires item kinds below 32. */
int Inv_TransferItemAlt2(int mask) {
    s16 *out = D_800A1E00;
    int i;
    D_8009D048 = D_800C0E48;
    D_8009D050 = Inv_GetAyaSlotLimit();
    D_8009D058 = D_8009D05C;
    D_8009D064 = 2;
    for (i = 0; i < D_8009D050; i++) {
        if ((mask >> ItemKind(i)) & 1) *out++ = i;
    }
    D_8009D044 = out - D_800A1E00;
    Inv_RebuildSelectableMask();
    for (i = 0; i < D_8009D050; i++) {
        if (D_8009D048[i] == 0) *out++ = i;
    }
    D_8009D044 = out - D_800A1E00;
    return D_8009D044;
}

int Inv_GetWayneListItemByIndex(int index) {
    if (index >= 0 && index < D_8009D044) {
        return D_800A1E00[index];
    }
    return 0;
}

/* Historical name: tests whether a slot may be acted on, excluding equipped
 * weapon/armor slots in Aya's list. An unresolved item remains allowed. */
int Inv_IsSlotEquipped(int index) {
    ItemDataRecord *data;
    int allowed = 1;
    int equipped, kind, count, i;
    data = LookupTrackedItem(index);
    if (data != 0) {
        allowed = 0;
        if (!(data->flags & 0x20)) {
            equipped = 0;
            if (D_8009D048 == D_800C0E48 &&
                (D_800C0E20.tracked[0] == index || D_800C0E20.tracked[2] == index))
                equipped = 1;
            if (!equipped) allowed = 1;
        }
        if (data->kind == 8) {
            kind = 8;
            count = 0;
            for (i = 0; i < D_8009D050; i++) {
                if (Inv_GetActiveListItemType(i) == kind) count++;
            }
            allowed &= count >= 2;
        }
    }
    return allowed;
}

/* Historical name: swaps inventory/storage entries after eligibility checks. */
int Inv_RebuildWithSlotLimit(int fromMode, int fromIndex, int toMode, int toIndex) {
    int success = 1;
    int from;
    D_8009D048 = D_800C0E48;
    D_8009D050 = Inv_GetAyaSlotLimit();
    D_8009D058 = D_8009D05C;
    D_8009D064 = 2;
    if (fromMode == 0x34 && toMode == fromMode) {
        SwapSlots(&D_8009D07C[fromIndex], &D_8009D07C[toIndex]);
        Inv_RebuildSelectableMask();
    } else if (fromMode == 0x33 && toMode == fromMode) {
        int first, second;
        first = WayneSlot(fromIndex);
        second = WayneSlot(toIndex);
        SwapAyaSlots(first, second);
        Inv_RebuildSelectableMask();
    } else if (fromMode == 0x34) {
        from = WayneSlot(toIndex);
        if (Inv_IsSlotEquipped(from) &&
            (LookupItem(D_8009D07C[fromIndex])->kind < 19 ||
             LookupItem(D_8009D07C[fromIndex])->kind >= 22)) {
            SwapSlots(&D_8009D07C[fromIndex], &D_8009D048[from]);
            Menu_SetSwapReturnFlag();
            Inv_RebuildSelectableMask();
        } else success = 0;
    } else {
        from = WayneSlot(fromIndex);
        if (Inv_IsSlotEquipped(from) &&
            (LookupItem(D_8009D07C[toIndex])->kind < 19 ||
             LookupItem(D_8009D07C[toIndex])->kind >= 22)) {
            SwapSlots(&D_8009D048[from], &D_8009D07C[toIndex]);
            Menu_SetSwapReturnFlag();
            Inv_RebuildSelectableMask();
        } else success = 0;
    }
    return success;
}
