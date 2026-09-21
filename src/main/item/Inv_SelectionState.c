/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_inventory.h"

static inline ItemDataRecord *LookupTrackedItem(int index) {
    int value, saved;
    ItemDataRecord *result;
    ItemDataRecord *output = 0;

    if (index >= 0 && index < D_8009D050) {
        value = D_8009D048[index];
        saved = value;
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
        output = result;
    }
    return output;
}


/* Historical names retained: snapshot, then restore the two selected records.
 * Both selected records must resolve, as in the retail copy sequences. */
void Inv_InitWayneStorage(void) {
    D_8009D084 = D_800A1FE8;
    D_8009D08C = 0;
    D_8009D088 = 0;
    D_800A204C = *LookupTrackedItem(Inv_RestoreSelection(0));
    D_800A206C = *LookupTrackedItem(Inv_RestoreSelection(1));
}

void Inv_RememberSelection(unsigned int index, int value) {
    if (index < 2) {
        g_InvSavedSelectionIndex[index] = value;
        g_InvSavedSelectionFromStorage[index] = g_InvItemPtr != g_AyaInventoryItems;
    }
}

static inline s32 RestoreSelection(u32 index) {
    if (index < 2U) {
        if ((g_InvSavedSelectionFromStorage[index] != 0) && (g_InvActiveListOverride != 0)) {
            g_InvItemPtr = g_InvActiveListOverride;
            g_InvSelectionBits = g_InvStorageSelectionBits;
            g_InvSelectionBitWords = 4;
            g_InvSlotLimit = g_InvOverrideSlotLimit;
        } else {
            g_InvItemPtr = g_AyaInventoryItems;
            g_InvSlotLimit = Inv_GetAyaSlotLimit();
            g_InvSelectionBits = g_AyaItemSelectionBits;
            g_InvSelectionBitWords = 2;
        }
        return g_InvSavedSelectionIndex[index];
    }
    return -1;
}

s32 Inv_RestoreSelection(u32 index) {
    return RestoreSelection(index);
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
void Inv_BuildStorageDisplay(void) {
    RestoreList(D_8009D098);
    *LookupTrackedItem(D_8009D090) = D_800A204C;
    RestoreList(D_8009D09C);
    *LookupTrackedItem(D_8009D094) = D_800A206C;
}

static inline ItemDataRecord *LookupActiveItem(int index) {
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
        return result;
    }
    return 0;
}


static inline void ClampAmmo(ItemDataRecord *item) {
    int current = item->ammo;
    if (item->baseStats[2] + item->bonusStats[2] < 1000 ?
        item->baseStats[2] + item->bonusStats[2] < current : 999 < current) item->ammo = (item->baseStats[2] + item->bonusStats[2] < 1000 ?
        item->baseStats[2] + (u16)item->bonusStats[2] : 999);
}
static inline void DeleteSlot(int slot) {
    int id = D_8009D048[slot];
    D_8009D048[slot] = 0;
    if (id >= 256) D_800C0E20.equipment[id - 256].pad_00[0] = 0;
}
static inline int ItemKind(int slot) {
    ItemDataRecord *item = LookupActiveItem(slot);
    if (item) return item->kind;
    return 0;
}
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
static inline int FindWeapon(int kind, int excluded) {
    s16 *slot = D_8009D048;
    s16 *skip = slot + excluded;
    while (slot < D_8009D048 + D_8009D050) {
        if (slot != skip && LookupItem(*slot)->kind == kind) break;
        ++slot;
    }
    if (slot < D_8009D048 + D_8009D050) return slot - D_8009D048;
    return -1;
}
static inline void SetTracked(unsigned index, int value) {
    if (index < 4) D_800C0E20.tracked[index] = value;
}
static inline int GetTracked(unsigned index) {
    return index < 4 ? D_800C0E20.tracked[index] : -1;
}
static inline int IsList(s16 *list) {
    return D_8009D048 == list;
}
/* Transfer bonuses or one modifier; flags govern tool and donor consumption.
 * Signed comparisons and halfword additions must remain distinct: the final
 * halfword store preserves modular arithmetic, including negative bonuses. */
void Inv_TransferItemBetweenLists(unsigned selection, int modifier, int flags, int cost) {
    ItemDataRecord *source, *dest, *ammo;
    int reserve;
    int slot, i, j, value, group, move;
    {
        int index = RestoreSelection(selection);
        source = 0;
        if (index >= 0 && index < D_8009D050) {
            source = LookupItem(D_8009D048[index]);
        }
    }
    {
        int index = RestoreSelection(!selection);
        dest = 0;
        if (index >= 0 && index < D_8009D050) {
            dest = LookupItem(D_8009D048[index]);
        }
    }
    if (modifier >= 0) {
        value = source->tailData[modifier];
        group = value & 0xE0;
        if (group) {
            unsigned key = group;
            for (i = 0; i < dest->tailCount; ++i)
                if (key == (dest->tailData[i] & 0xE0)) break;
            if (i < dest->tailCount) {
                dest->tailData[i] = value;
                source->tailData[modifier] = 0;
            } else {
                for (j = 0; j < dest->tailCount; ++j)
                    if (!(dest->tailData[j] & 31)) break;
                if (j < dest->tailCount) {
                    dest->tailData[j] = source->tailData[modifier];
                    source->tailData[modifier] = 0;
                } else Menu_CreateNotificationDialog(7, 0);
            }
        } else {
            for (j = 0; j < dest->tailCount; ++j)
                if (!(dest->tailData[j] & 31)) break;
            if (j < dest->tailCount) {
                dest->tailData[j] = source->tailData[modifier];
                source->tailData[modifier] = 0;
            } else Menu_CreateNotificationDialog(7, 0);
        }
    } else {
#define TRANSFER_STAT(n) dest->bonusStats[n] = source->bonusStats[n] + dest->bonusStats[n] < 1000 ? (u16)source->bonusStats[n] + (u16)dest->bonusStats[n] : 999
        TRANSFER_STAT(0);
        TRANSFER_STAT(1);
        TRANSFER_STAT(2);
#undef TRANSFER_STAT
        source->bonusStats[0] = 0;
        source->bonusStats[1] = 0;
        source->bonusStats[2] = 0;
        if (source->kind != 9 && source->ammo > source->baseStats[2]) {
            int excess = source->ammo - source->baseStats[2];
            int loaded;
            int rawCapacity = dest->baseStats[2] + dest->bonusStats[2];
            loaded = dest->ammo;
            if (rawCapacity < 1000 ? rawCapacity - loaded < excess : 999 - loaded < excess) {
                int loaded;
                int rawCapacity = dest->baseStats[2] + dest->bonusStats[2];
                loaded = dest->ammo;
                move = rawCapacity < 1000 ? rawCapacity - loaded : 999 - loaded;
            }
            else move = source->ammo - source->baseStats[2];
            dest->ammo += move;
            {
                unsigned kind = source->kind;
                if (kind != 0 && kind < 8) {
                    ammo = D_800A1E64;
                    if ((int)kind - 4 > 0) ammo = (ItemDataRecord *)((u8 *)D_800A1E64 + (((int)kind - 5) << 5));
                } else if (kind >= 19) ammo = (ItemDataRecord *)((u8 *)D_800A1E64 + (((int)kind - 19) << 5));
                else ammo = &D_800A1E44;
            }
            ammo->ammo += source->ammo - source->baseStats[2] - move;
            ClampAmmo(ammo);
            source->ammo = source->baseStats[2];
        }
    }
    if (cost < 999) {
        cost = (flags & 1) ? 13 : 12;
        if (flags & 2) {
            for (slot = 0; slot < 100; ++slot)
                if (D_800C1EB8[slot] && Item_LookupBaseData(D_800C1EB8[slot] - 1)->kind == cost) break;
            if (slot < 100) D_800C1EB8[slot] = 0;
        } else {
            D_8009D048 = D_800C0E48;
            D_8009D050 = Inv_GetAyaSlotLimit();
            D_8009D058 = D_8009D05C;
            D_8009D064 = 2;
            for (slot = 0; slot < D_8009D050; ++slot)
                if (ItemKind(slot) == cost) break;
            if (slot < D_8009D050) {
                if (D_8009D048 == D_800C0E48 && GetTracked(2) == slot)
                    Inv_GetActiveSlotCount(&reserve);
                DeleteSlot(slot);
                if (D_8009D048 == D_800C0E48 && GetTracked(2) == slot) {
                    SetTracked(2, -1);
                    Inv_CheckFreeSlotCapacity(reserve);
                    Inv_CompactActiveListSlots();
                    Inv_SetActiveList(3, 0);
                }
            }
        }
    }
    if (!(flags & 1)) {
        slot = RestoreSelection(selection);
        {
            unsigned kind = source->kind;
            if (kind != 0 && kind < 8) {
                ammo = D_800A1E64;
                if ((int)kind - 4 > 0) ammo = (ItemDataRecord *)((u8 *)D_800A1E64 + (((int)kind - 5) << 5));
            } else if (kind >= 19) ammo = (ItemDataRecord *)((u8 *)D_800A1E64 + (((int)kind - 19) << 5));
            else ammo = &D_800A1E44;
        }
        ammo->ammo += source->ammo;
        ClampAmmo(ammo);
        if (D_8009D048 == D_800C0E48 && GetTracked(2) == slot)
            Inv_GetActiveSlotCount(&reserve);
        DeleteSlot(slot);
        if (IsList(D_800C0E48)) {
            if (GetTracked(2) == slot) {
                SetTracked(2, -1);
                Inv_CheckFreeSlotCapacity(reserve);
                Inv_CompactActiveListSlots();
                Inv_SetActiveList(3, 0);
            }
            if (IsList(D_800C0E48)) {
                if (D_800C0E20.tracked[0] == slot) {
                    D_800C0E20.tracked[0] = FindWeapon(8, -1);
                    Inv_SetActiveList(2, 0);
                } else if (GetTracked(2) == slot) {
                    for (slot = 0; slot < D_8009D050; ++slot) {
                        ItemDataRecord *item = LookupActiveItem(slot);
                        if (item && item->kind == 9) break;
                    }
                    if (slot >= D_8009D050) slot = -1;
                    D_800C0E20.tracked[2] = slot;
                    Inv_SetActiveList(3, 0);
                }
            }
        }
    }
}
