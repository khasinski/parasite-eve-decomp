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

static inline int FindActiveSlot(int value) {
    /* Matching constraint: retail keeps the search key in $a3. */
    register int key asm("$7") = value;
    s16 *p = D_8009D048, *end = p + D_8009D050;
    if (p < end) {
        do {
            if (*p == key)
                break;
            p++;
        } while (p < end);
        if (p < D_8009D048 + D_8009D050)
            return p - D_8009D048;
    }
    return -1;
}
static inline void ClampAmmo(ItemDataRecord *item) {
    int current = item->ammo;
    if (item->baseStats[2] + item->bonusStats[2] < 1000 ?
        item->baseStats[2] + item->bonusStats[2] < current : 999 < current) {
        item->ammo = item->baseStats[2] + item->bonusStats[2] < 1000 ?
            item->baseStats[2] + (u16)item->bonusStats[2] : 999;
    }
}

/* Add loaded ammunition even when no empty inventory slot is available. */
int Inv_WriteSlotById(ItemDataRecord *item) {
    unsigned kind = item->kind;
    int failed = 0;
    int category;

    if (kind < 16) {
        if (kind && kind < 8)
            category = (int)(kind - 4) > 0 ? kind - 5 : 0;
        else
            category = kind >= 19 ? kind - 19 : -1;
    } else {
        category = kind - 16;
    }
    if ((unsigned)category < 3) {
        ItemDataRecord *pool;
        if (FindActiveSlot(category + 512) < 0) {
            int slot = FindFreeActiveSlot();
            if (slot >= 0)
                D_8009D048[slot] = category + 512;
            else
                failed = 1;
        }
        pool = &D_800A1E64[category];
        pool->ammo += item->ammo;
        ClampAmmo(pool);
    } else {
        failed = 1;
    }
    return failed;
}
