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

static inline int ActiveItemKind(int index) {
    ItemDataRecord *item;
    if (index >= 0 && index < D_8009D050)
        item = LookupItem(D_8009D048[index]);
    else
        item = 0;
    return item ? item->kind : 0;
}
static inline int FindFreeEquipmentRecord(void) {
    int result;
    ItemDataRecord *p = D_800C0E20.equipment;
    ItemDataRecord *end;
    /* Preserve retail's independent base reload beside the cached bound. */
    asm("" : "=r"(p) : "0"(p));
    end = p + 128;

    if (p < D_800C0E20.equipment + 128) {
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

/* Replace base equipment IDs with private, mutable equipment records. */
void Inv_ClearDisplaySlots(void) {
    int i = 0;
    g_InvActiveListOverride = 0;
    g_InvOverrideSlotLimit = 0;
    D_8009D048 = D_800C0E20.slots;
    D_8009D050 = Inv_GetAyaSlotLimit();
    D_8009D058 = D_8009D05C;
    D_8009D064 = 2;
    for (; i < D_8009D050; i++) {
        if ((unsigned)(ActiveItemKind(i) - 1) < 9 &&
            (unsigned)((u16)D_8009D048[i] - 256) >= 128) {
            int record = FindFreeEquipmentRecord();
            if (record >= 0) {
                ItemDataRecord *item = &D_800C0E20.equipment[record];
                *item = *Item_LookupBaseData(D_8009D048[i] - 1);
                D_8009D048[i] = record + 256;
            }
        }
    }
}

/* Select a storage list, adding the special item ID when needed. */
int Inv_StepScrollDisplay2(int special) {
    if (special) {
        int i, found;
        D_8009D07C = D_800C1F80;
        for (i = 0; i < 82; i++)
            if (D_8009D07C[i] == 516)
                break;
        found = i < 82;
        if (!found) {
            for (i = 0; i < D_8009D050; i++) {
                ItemDataRecord *item;
                if (i >= 0 && i < D_8009D050)
                    item = LookupItem(D_8009D048[i]);
                else
                    item = 0;
                if (item && item->kind == 6)
                    break;
            }
            if (i < D_8009D050) {
                /* If slots 0..80 are occupied, retail writes slot 81 unchecked. */
                for (i = 0; i < 81; i++)
                    if (!D_8009D07C[i])
                        break;
                if (i < 82) {
                    D_8009D07C[i] = 516;
                    found = 1;
                }
            }
        }
        D_8009D080 = 80 + found;
    } else {
        D_8009D07C = D_800C1EB8;
        D_8009D080 = 100;
    }
    g_InvActiveListOverride = D_8009D07C;
    return g_InvOverrideSlotLimit = D_8009D080;
}
