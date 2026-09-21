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

static inline void RestoreList(int storage) {
    if (storage && g_InvActiveListOverride != 0) {
        D_8009D048 = g_InvActiveListOverride;
        D_8009D058 = g_InvStorageSelectionBits;
        D_8009D064 = 4;
        D_8009D050 = g_InvOverrideSlotLimit;
    } else {
        D_8009D048 = D_800C0E48;
        D_8009D050 = Inv_GetAyaSlotLimit();
        D_8009D058 = D_8009D05C;
        D_8009D064 = 2;
    }
}

static inline int ListCount(void) {
    if (g_InvActiveListOverride) return 2;
    return 1;
}

static inline void ClearBits(void) {
    int i;
    for (i = 0; i < D_8009D064; i++) D_8009D058[i] = 0;
}

static inline int CountBits(void) {
    int i, count = 0;
    for (i = 0; i < D_8009D050; i++)
        count += (D_8009D058[i >> 5] & (1u << (i & 31))) > 0;
    return count;
}

/* Build ammunition-compatible selections in the current list and, when present,
 * the other list. Exclude sourceIndex only in the original list. The source
 * slot must resolve to an item. The return value counts both selections. */
int Inv_BuildCompatibleWeaponBitset(int sourceIndex) {
    ItemDataRecord *item;
    unsigned kind;
    int category, i, count, wasStorage;
    if (sourceIndex >= 0 && sourceIndex < D_8009D050) item = LookupItem(D_8009D048[sourceIndex]);
    else item = 0;

    kind = item->kind;
    if (kind != 0 && kind < 8) {
        category = (int)kind - 4;
        if (category <= 0) category = 1;
    } else {
        if (item->kind >= 19) category = item->kind - 18;
        else category = 0;
    }
    ClearBits();
    if ((unsigned)(kind - 19) < 3) {
        for (i = 0; i < D_8009D050; i++) {
            if (i != sourceIndex) {
                if (i >= 0 && i < D_8009D050) item = LookupItem(D_8009D048[i]);
                else item = 0;
                if (item) {
                    D_8009D058[i >> 5] |= (u32)(category == (item->kind && item->kind < 8
                        ? ((int)item->kind - 4 > 0 ? (int)item->kind - 4 : 1)
                        : item->kind >= 19 ? item->kind - 18 : 0)) << (i & 31);
                }
            }
        }
    } else {
        for (i = 0; i < D_8009D050; i++) {
            if (i != sourceIndex) {
                if (i >= 0 && i < D_8009D050) item = LookupItem(D_8009D048[i]);
                else item = 0;
                if (item) {
                    u16 candidateKind = item->kind;
                    D_8009D058[i >> 5] |= (u32)((unsigned)(candidateKind - 19) < 3 &&
                        category == (item->kind && item->kind < 8
                        ? ((int)item->kind - 4 > 0 ? (int)item->kind - 4 : 1)
                        : item->kind >= 19 ? item->kind - 18 : 0)) << (i & 31);
                }
            }
        }
    }
    count = CountBits();
    wasStorage = D_8009D048 != D_800C0E48;
    if (wasStorage || ListCount() == 2) {
        RestoreList(!wasStorage);
        ClearBits();
        if ((unsigned)(kind - 19) < 3) {
            for (i = 0; i < D_8009D050; i++) {
                if (i >= 0 && i < D_8009D050) item = LookupItem(D_8009D048[i]);
                else item = 0;
                if (item) {
                    D_8009D058[i >> 5] |= (u32)(category == (item->kind && item->kind < 8
                        ? ((int)item->kind - 4 > 0 ? (int)item->kind - 4 : 1)
                        : item->kind >= 19 ? item->kind - 18 : 0)) << (i & 31);
                }
            }
        } else {
            for (i = 0; i < D_8009D050; i++) {
                if (i >= 0 && i < D_8009D050) item = LookupItem(D_8009D048[i]);
                else item = 0;
                if (item) {
                    u16 candidateKind = item->kind;
                    D_8009D058[i >> 5] |= (u32)((unsigned)(candidateKind - 19) < 3 &&
                        category == (item->kind && item->kind < 8
                        ? ((int)item->kind - 4 > 0 ? (int)item->kind - 4 : 1)
                        : item->kind >= 19 ? item->kind - 18 : 0)) << (i & 31);
                }
            }
        }
        count += CountBits();
        RestoreList(wasStorage);
    }
    return count;
}
