/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory_slots.h"
#include "pe1/random.h"
#include "pe1/text.h"
static inline ItemDataRecord *LookupItem(int value) {
    int saved = value;
    ItemDataRecord *result;
    if ((unsigned)(value - 0x100) < 0x80) {
        result = &D_800C0E20.equipment[value - 0x100];
    } else {
        if ((unsigned)(value - 1) < 0xFF) {
            result = Item_LookupBaseData(value - 1);
        } else if ((unsigned)(saved - 0x200) < 9) {
            result = &D_800A1E64[saved - 0x200];
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


static inline int Eligible(int index,int mask) {
    ItemDataRecord *item;
    if (index >= 0 && index < D_8009D050) item=LookupItem(D_8009D048[index]);
    else item=0;
    return item && ((mask >> item->kind) & 1) && item->tailCount != 0;
}
int Inv_SetupSlotDisplay(int mask) {
    int i=0, count=0;
    int wasStorage=D_8009D048 != D_800C0E48;
    RestoreList(0);
    for (; i<D_8009D050; ++i) count += Eligible(i,mask);
    if (ListCount() >= 2 && !(mask & 0x100)) {
        RestoreList(1);
        for (i=0; i<D_8009D050; ++i) count += Eligible(i,mask);
    }
    RestoreList(wasStorage);
    return count>=2;
}

static inline int RandomByte(void) {
    int i = 0;
    if (++D_8009D038 >= 521) {
        for (; i < 32; ++i)
            D_800A1B90[i] ^= D_800A1B90[i + 489];
        for (i = 32; i < 521; ++i)
            D_800A1B90[i] ^= D_800A1B90[i - 32];
        D_8009D038 = 0;
    }
    return D_800A1B90[D_8009D038];
}

static inline int IsTracked(int index) {
    int result = 0;
    if (D_8009D048 == D_800C0E48) {
        if (D_800C0E20.tracked[0] == index || D_800C0E20.tracked[2] == index)
            result = 1;
    }
    return result;
}
int Inv_PickRandomItem(int unused) {
    s16 *out = D_800A1D9C;
    int i;
    ItemDataRecord *item;
    for (i = 0; i < D_8009D050; ++i) {
        item = LookupActiveItem(i);
        if (item && (item->flags & 128) && !IsTracked(i)) *out++ = i;
    }
    D_8009D068 = 0;
    D_8009D040 = out - D_800A1D9C;
    if (!D_8009D040) return 0;
    return D_8009D048[D_800A1D9C[(D_8009D040 * RandomByte()) >> 8]];
}

/* Historical name: formats the item message into the shared FF-terminated buffer. */
static inline void CopyText(u8 *dest, const u8 *src) {
    while ((*dest++ = *src++) != 255) {}
}
static inline void AppendText(u8 *dest, const u8 *src) {
    while (*dest++ != 255) {}
    --dest;
    while ((*dest++ = *src++) != 255) {}
}
u8 *Inv_GetItemEffectData(int item, int mode) {
    if (!item) CopyText(D_800A1B50, Str_LookupTable4(19));
    else if (mode >= 2) CopyText(D_800A1B50, Str_LookupTable4(18));
    else if (!D_8009D218) {
        CopyText(D_800A1B50, Str_LookupTable8(item - 1));
        AppendText(D_800A1B50, Str_LookupTable4(mode + 16));
    } else {
        CopyText(D_800A1B50, Str_LookupTable4(mode + 16));
        AppendText(D_800A1B50, Str_LookupTable8(item - 1));
    }
    return D_800A1B50;
}
