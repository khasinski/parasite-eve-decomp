/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/aya.h"
#include "pe1/battle_cmd.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory_sort.h"
#include "pe1/inventory_state.h"
#include "pe1/menu_state.h"
#include "pe1/psyq_libc.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
s32 g_InvPackedListCount;
s16 *g_InvItemPtr;
s32 g_InvSlotLimit;
u32 *g_InvSelectionBits;
s32 g_InvSelectionBitWords;
u8 *g_InvScratchBufferPtrWriteOnly;
s32 g_InvScratchWordAWriteOnly;
s32 g_InvScratchWordBWriteOnly;
s32 g_InvSavedSelectionIndex0;
s32 g_InvSavedSelectionIndex1;
s32 g_InvSavedSelectionFromStorage0;
s32 g_InvSavedSelectionFromStorage1;
s16 *g_InvActiveListOverride;
s32 g_InvOverrideSlotLimit;
s32 g_InvWeaponSortStatIndex;
s32 g_InvAmmoSortStatIndex;
unsigned char *g_InvSortRankTable;
unsigned char *g_InvSortTypeRankTable;
u16 *g_InvSortListBase;
s32 g_InvSortListCount;
ItemDataRecord *(*g_InvSortLookup)(int index);
int g_InvSortStatIndex;
int g_MenuBattleEquipMode;

static inline ItemDataRecord *LookupOldArmor(int index) {
    int value, saved;
    ItemDataRecord *result;

    if (index >= 0 && index < g_InvSlotLimit) {
        value = g_InvItemPtr[index];
        saved = value;
        if (value - 0x100U < 0x80U) {
            result = &g_InventoryRuntime.equipment[value - 0x100];
        } else {
            if (value - 1U < 0xFFU) {
                return Item_LookupBaseData(value - 1);
            }
            if (saved - 0x200U < 9U) {
                result = &((ItemDataRecord *)g_KeyItemDataTable)[saved];
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

static inline ItemDataRecord *LookupTrackedItem(int index) {
    int value, saved;
    ItemDataRecord *result;
    ItemDataRecord *output = 0;

    if (index >= 0 && index < g_InvSlotLimit) {
        value = g_InvItemPtr[index];
        saved = value;
        if (value - 0x100U < 0x80U) {
            result = &g_InventoryRuntime.equipment[value - 0x100];
        } else {
            if (value - 1U < 0xFFU) {
                result = Item_LookupBaseData(value - 1);
            } else if (saved - 0x200U < 9U) {
                result = &((ItemDataRecord *)g_KeyItemDataTable)[saved];
            } else {
                result = 0;
            }
        }
        output = result;
    }
    return output;
}

static inline ItemDataRecord *LookupArmorTrackedItem(int index) {
    int value, saved;
    ItemDataRecord *result;
    /* Match the shared armor pointer across reserve scanning and rollback. */
    register ItemDataRecord *output asm("$16") = 0;

    if (index >= 0 && index < g_InvSlotLimit) {
        value = g_InvItemPtr[index];
        saved = value;
        if (value - 0x100U < 0x80U) {
            result = &g_InventoryRuntime.equipment[value - 0x100];
        } else {
            if (value - 1U < 0xFFU) {
                result = Item_LookupBaseData(value - 1);
            } else if (saved - 0x200U < 9U) {
                result = &((ItemDataRecord *)g_KeyItemDataTable)[saved];
            } else {
                result = 0;
            }
        }
        output = result;
    }
    return output;
}

static inline int FilteredSlot(int index) {
    int slot;
    if (index >= 0 && index < g_InvPackedListCount) slot = g_InvPackedList[index];
    else slot = 0;
    return slot;
}

/* Historical name: switch armor using a filtered-list index, returning status. */
int Inv_GetSlotItemData(int index) {
    ItemDataRecord *armor;
    int previousReserve = 0;
    int nextReserve, previousSlot, status, i;
    BattleCmdEntry *command;
    armor = LookupOldArmor(g_InventoryRuntime.tracked[2]);
    if (armor != 0) {
        for (i = 0; i < armor->tailCount; i++) {
            previousReserve = (armor->tailData[i] & 31) - 8;
            if ((unsigned)previousReserve < 3) break;
        }
        if (i < armor->tailCount) previousReserve = 1 << previousReserve;
        else previousReserve = 0;
    }
    armor = LookupTrackedItem(FilteredSlot(index));
    for (i = 0; i < armor->tailCount; i++) {
        nextReserve = (armor->tailData[i] & 31) - 8;
        if ((unsigned)nextReserve < 3) break;
    }
    if (i < armor->tailCount) nextReserve = 1 << nextReserve;
    else nextReserve = 0;
    previousSlot = g_InventoryRuntime.tracked[2];
    armor = LookupArmorTrackedItem(previousSlot);
    g_InventoryRuntime.tracked[2] = FilteredSlot(index);
    status = Inv_CheckFreeSlotCapacity(previousReserve - nextReserve);
    if (status == 1) {
        command = BattleCmd_AllocSlot();
        command->header.word = 3;
        command->payload.equip_restore.item_data = armor;
        Inv_SetActiveList(3, 0);
    } else {
        g_InventoryRuntime.tracked[2] = previousSlot;
    }
    return status;
}

/* Historical name: checks free capacity against the armor-derived reserve.
 * Also selects Aya's inventory and optionally returns the required reserve. */
int Inv_GetActiveSlotCount(int *out) {
    ItemDataRecord *armor;
    int reserve = 0;
    int used;
    int capacity, i, enough, occupied;
    int capacityBeforeScan, bonusSlots;
    s16 *slot, *end;

    armor = LookupOldArmor(g_InventoryRuntime.tracked[2]);
    if (armor != 0) {
        for (i = 0; i < armor->tailCount; i++) {
            reserve = (armor->tailData[i] & 0x1F) - 8;
            if ((unsigned)reserve < 3) break;
        }
        if (i < armor->tailCount) reserve = 1 << reserve;
        else reserve = 0;
    }
    bonusSlots = Inv_GetBonusSlotCount();
    if (g_InvBaseCapacity + bonusSlots < 51) {
        used = 0;
        bonusSlots = Inv_GetBonusSlotCount();
        capacityBeforeScan = g_InvBaseCapacity + bonusSlots;
    } else {
        used = 0;
        capacityBeforeScan = 50;
    }
    g_InvItemPtr = g_AyaInventoryItems;
    capacity = capacityBeforeScan;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    slot = g_InvItemPtr;
    end = slot + g_InvSlotLimit;
    while (slot < end) {
        occupied = *slot++;
        occupied = occupied != 0;
        used += occupied;
    }
    enough = capacity - used >= reserve;
    if (out != 0) *out = reserve;
    return enough;
}

/* Historical names retained: snapshot, then restore the two selected records.
 * Both selected records must resolve, as in the retail copy sequences. */
void Inv_InitWayneStorage(void) {
    g_InvScratchBufferPtrWriteOnly = g_InvScratchBufferUnused;
    g_InvScratchWordBWriteOnly = 0;
    g_InvScratchWordAWriteOnly = 0;
    g_InvSavedSelectionRecord0 = *LookupTrackedItem(Inv_RestoreSelection(0));
    g_InvSavedSelectionRecord1 = *LookupTrackedItem(Inv_RestoreSelection(1));
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
            g_InvSelectionBits = g_InvSelectionBitStorage;
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
        g_InvItemPtr = g_InvActiveListOverride;
        g_InvSelectionBits = g_InvStorageSelectionBits;
        g_InvSelectionBitWords = 4;
        g_InvSlotLimit = g_InvOverrideSlotLimit;
    } else {
        g_InvItemPtr = g_AyaInventoryItems;
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
        g_InvSelectionBits = g_InvSelectionBitStorage;
        g_InvSelectionBitWords = 2;
    }
}
void Inv_BuildStorageDisplay(void) {
    RestoreList(g_InvSavedSelectionFromStorage0);
    *LookupTrackedItem(g_InvSavedSelectionIndex0) = g_InvSavedSelectionRecord0;
    RestoreList(g_InvSavedSelectionFromStorage1);
    *LookupTrackedItem(g_InvSavedSelectionIndex1) = g_InvSavedSelectionRecord1;
}

static inline ItemDataRecord *LookupActiveItem(int index) {
    int value, saved;
    ItemDataRecord *result;

    if (index >= 0 && index < g_InvSlotLimit) {
        value = g_InvItemPtr[index];
        saved = value;
        if (value - 0x100U < 0x80U) {
            result = &g_InventoryRuntime.equipment[value - 0x100];
        } else {
            if (value - 1U < 0xFFU) {
                return Item_LookupBaseData(value - 1);
            }
            if (saved - 0x200U < 9U) {
                result = &((ItemDataRecord *)g_KeyItemDataTable)[saved];
            } else {
                result = 0;
            }
        }
        return result;
    }
    return 0;
}

/* Ammunition pool record. An inline call keeps the index scaled before the
 * table address is added, as retail does; a direct &g_InvSpecialItemRecords[kind - 5]
 * folds the -5 into the address (lev 16). */
static inline ItemDataRecord *AmmoPoolRecord(int index) {
    return &g_InvSpecialItemRecords[index];
}
static inline void ClampAmmo(ItemDataRecord *item) {
    int current = item->ammo;
    if (item->baseStats[2] + item->bonusStats[2] < 1000 ?
        item->baseStats[2] + item->bonusStats[2] < current : 999 < current) item->ammo = (item->baseStats[2] + item->bonusStats[2] < 1000 ?
        item->baseStats[2] + (u16)item->bonusStats[2] : 999);
}
static inline void DeleteSlot(int slot) {
    int id = g_InvItemPtr[slot];
    g_InvItemPtr[slot] = 0;
    if (id >= 256) g_InventoryRuntime.equipment[id - 256].iconId = 0;
}
static inline int ItemKind(int slot) {
    ItemDataRecord *item = LookupActiveItem(slot);
    if (item) return item->kind;
    return 0;
}
static inline int FindWeapon(int kind, int excluded) {
    s16 *slot = g_InvItemPtr;
    s16 *skip = slot + excluded;
    while (slot < g_InvItemPtr + g_InvSlotLimit) {
        if (slot != skip && Inv_LookupItemRecordScaled(*slot)->kind == kind) break;
        ++slot;
    }
    if (slot < g_InvItemPtr + g_InvSlotLimit) return slot - g_InvItemPtr;
    return -1;
}
static inline void SetTracked(unsigned index, int value) {
    if (index < 4) g_InventoryRuntime.tracked[index] = value;
}
static inline int GetTracked(unsigned index) {
    return index < 4 ? g_InventoryRuntime.tracked[index] : -1;
}
static inline int IsList(s16 *list) {
    return g_InvItemPtr == list;
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
        if (index >= 0 && index < g_InvSlotLimit) {
            source = Inv_LookupItemRecordScaled(g_InvItemPtr[index]);
        }
    }
    {
        int index = RestoreSelection(!selection);
        dest = 0;
        if (index >= 0 && index < g_InvSlotLimit) {
            dest = Inv_LookupItemRecordScaled(g_InvItemPtr[index]);
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
                    ammo = g_InvSpecialItemRecords;
                    if ((int)kind - 4 > 0) ammo = AmmoPoolRecord((int)kind - 5);
                } else if (kind >= 19) ammo = AmmoPoolRecord((int)kind - 19);
                else ammo = &g_InvFallbackAmmoRecord;
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
                if (g_WayneStorageItems[slot] && Item_LookupBaseData(g_WayneStorageItems[slot] - 1)->kind == cost) break;
            if (slot < 100) g_WayneStorageItems[slot] = 0;
        } else {
            g_InvItemPtr = g_AyaInventoryItems;
            g_InvSlotLimit = Inv_GetAyaSlotLimit();
            g_InvSelectionBits = g_InvSelectionBitStorage;
            g_InvSelectionBitWords = 2;
            for (slot = 0; slot < g_InvSlotLimit; ++slot)
                if (ItemKind(slot) == cost) break;
            if (slot < g_InvSlotLimit) {
                if (g_InvItemPtr == g_AyaInventoryItems && GetTracked(2) == slot) {
                    Inv_GetActiveSlotCount(&reserve);
                    Inv_CheckFreeSlotCapacity(reserve);
                    SetTracked(2, -1);
                    Inv_SetActiveList(3, 0);
                }
                DeleteSlot(slot);
            }
        }
    }
    if (!(flags & 1)) {
        slot = RestoreSelection(selection);
        {
            unsigned kind = source->kind;
            if (kind != 0 && kind < 8) {
                ammo = g_InvSpecialItemRecords;
                if ((int)kind - 4 > 0) ammo = AmmoPoolRecord((int)kind - 5);
            } else if (kind >= 19) ammo = AmmoPoolRecord((int)kind - 19);
            else ammo = &g_InvFallbackAmmoRecord;
        }
        ammo->ammo += source->ammo;
        ClampAmmo(ammo);
        if (g_InvItemPtr == g_AyaInventoryItems && GetTracked(2) == slot) {
            Inv_GetActiveSlotCount(&reserve);
            Inv_CheckFreeSlotCapacity(reserve);
            SetTracked(2, -1);
            Inv_SetActiveList(3, 0);
        }
        DeleteSlot(slot);
        if (IsList(g_AyaInventoryItems)) {
            if (g_InventoryRuntime.tracked[0] == slot) {
                g_InventoryRuntime.tracked[0] = FindWeapon(8, -1);
                Inv_SetActiveList(2, 0);
            } else if (GetTracked(2) == slot) {
                for (slot = 0; slot < g_InvSlotLimit; ++slot) {
                    ItemDataRecord *item = LookupActiveItem(slot);
                    if (item && item->kind == 9) break;
                }
                if (slot >= g_InvSlotLimit) slot = -1;
                g_InventoryRuntime.tracked[2] = slot;
                Inv_SetActiveList(3, 0);
            }
        }
    }
}

int Inv_CompareItemsForSort(const void *leftElement, const void *rightElement) {
    const short *leftIndex = leftElement;
    const short *rightIndex = rightElement;
    ItemDataRecord *left;
    ItemDataRecord *right;
    int leftKind;
    int rightKind;
    int leftSubtype;
    int rightSubtype;
    int leftName;
    int rightName;
    int leftItem;
    int rightItem;
    ItemDataRecord *(*defaultLookup)(int);
    ItemDataRecord *(*lookup)(int);
    defaultLookup = Inv_LookupActiveListData;
    asm volatile("" ::: "memory");
    leftItem = *leftIndex;
    rightItem = *rightIndex;
    lookup = g_InvSortLookup;
    if (!(lookup != defaultLookup && leftItem == 0)) {
    if (lookup != defaultLookup) {
        if (rightItem == 0) {
            return -1;
        }
    }

    left = lookup(leftItem);
    right = g_InvSortLookup(rightItem);
    leftKind = g_InvSortRankTable[left->kind];
    rightKind = g_InvSortRankTable[right->kind];
    leftSubtype = g_InvSortTypeRankTable[((unsigned char *)left)[0xE] & 0xF];
    rightSubtype = g_InvSortTypeRankTable[((unsigned char *)right)[0xE] & 0xF];

    if (left != 0) {
        if (right == 0) {
            return -1;
        }
        asm volatile("" ::: "memory");

    } else {
    if (right != 0) {
        return 1;
    }
    }
    if (rightKind < leftKind) {
        return 1;
    }
    if (leftKind < rightKind) {
        return -1;
    }
    if (rightSubtype < leftSubtype) {
        return 1;
    }
    if (leftSubtype < rightSubtype) {
        return -1;
    }
    leftName = left->itemId;
    rightName = right->itemId;
    return (unsigned int)rightName < (unsigned int)leftName
               ? 1
               : -((unsigned int)leftName < (unsigned int)rightName);

    }
    return rightItem != 0;
}

int Inv_CompareItemsByStatForSort(const void *leftElement, const void *rightElement)
{
    s16 *leftIndex = (s16 *)leftElement;
    s16 *rightIndex = (s16 *)rightElement;
    ItemDataRecord *left;
    ItemDataRecord *right;
    int leftValue;
    int rightValue;
    int leftName;
    int rightName;

    leftValue = 0;
    rightValue = 0;
    left = g_InvSortLookup(*leftIndex);
    right = g_InvSortLookup(*rightIndex);

    switch (g_InvSortStatIndex) {
    case 0:
        leftValue = left->baseStats[0] + left->bonusStats[0];
        rightValue = right->baseStats[0] + right->bonusStats[0];
        break;
    case 1:
        leftValue = left->baseStats[1] + left->bonusStats[1];
        rightValue = right->baseStats[1] + right->bonusStats[1];
        break;
    case 2:
        leftValue = left->baseStats[2] + left->bonusStats[2];
        rightValue = right->baseStats[2] + right->bonusStats[2];
        break;
    }

    if (rightValue < leftValue) {
        return -1;
    }
    if (leftValue < rightValue) {
        return 1;
    }

    leftName = left->itemId;
    rightName = right->itemId;
    return (unsigned int)rightName < (unsigned int)leftName
               ? 1
               : -((unsigned int)leftName < (unsigned int)rightName);
}

void Inv_SortWeaponSubrange(void) {
    u16 *var_s0;
    u16 *var_s1;
    s32 mask;
    s32 var_a0;
    u16 tmpu;

    var_s0 = g_InvSortListBase;
    g_InvSortStatIndex = g_InvWeaponSortStatIndex;
    mask = 0x1FE;
    if (var_s0 < g_InvSortListBase + g_InvSortListCount) {
        while (*(s16 *) var_s0 != 0) {
            if ((mask >> g_InvSortLookup((s16)*var_s0)->kind) & 1) {
                break;
            }
            var_s0 += 1;
            if (!(var_s0 < g_InvSortListBase + g_InvSortListCount)) {
                return;
            }
        }
        if (var_s0 < g_InvSortListBase + g_InvSortListCount) {
            tmpu = *var_s0;
            var_s1 = var_s0;
            if (*(s16 *) var_s0 != 0) {
                do {
                    var_a0 = tmpu << 0x10;
                    if (!((mask >> g_InvSortLookup((s16)(var_a0 >> 0x10))->kind) & 1)) {
                        break;
                    }
                    var_s0 += 1;
                    if (!(var_s0 < g_InvSortListBase + g_InvSortListCount)) {
                        break;
                    }
                    tmpu = *var_s0;
                } while (*(s16 *) var_s0 != 0);
                qsort(var_s1, (var_s0 - var_s1), 2, &Inv_CompareItemsByStatForSort);
            }
        }
    }
}

void Inv_SortAmmoSubrange(void) {
    u16 *var_s0;
    u16 *var_s1;
    s32 mask;
    s32 var_a0;
    u16 tmpu;

    var_s0 = g_InvSortListBase;
    g_InvSortStatIndex = g_InvAmmoSortStatIndex;
    mask = 0x200;
    if (var_s0 < g_InvSortListBase + g_InvSortListCount) {
        while (*(s16 *) var_s0 != 0) {
            if ((mask >> g_InvSortLookup((s16)*var_s0)->kind) & 1) {
                break;
            }
            var_s0 += 1;
            if (!(var_s0 < g_InvSortListBase + g_InvSortListCount)) {
                return;
            }
        }
        if (var_s0 < g_InvSortListBase + g_InvSortListCount) {
            tmpu = *var_s0;
            var_s1 = var_s0;
            if (*(s16 *) var_s0 != 0) {
                do {
                    var_a0 = tmpu << 0x10;
                    if (!((mask >> g_InvSortLookup((s16)(var_a0 >> 0x10))->kind) & 1)) {
                        break;
                    }
                    var_s0 += 1;
                    if (!(var_s0 < g_InvSortListBase + g_InvSortListCount)) {
                        break;
                    }
                    tmpu = *var_s0;
                } while (*(s16 *) var_s0 != 0);
                qsort(var_s1, (var_s0 - var_s1), 2, &Inv_CompareItemsByStatForSort);
            }
        }
    }
}

void Inv_SortInventoryByMode(s32 arg0, s32 arg1) {
    s16 *var_a0;
    s16 *var_a0_2;
    s32 temp_s1;
    s32 var_s0;
    s32 var_v1;
    s32 var_v1_2;

    switch (arg0) {
    case 0:
        g_InvSortRankTable = g_InvSortRankTableC;
        g_InvAmmoSortStatIndex = arg1;
        break;
    case 1:
        g_InvSortRankTable = g_InvSortRankTableA;
        g_InvWeaponSortStatIndex = arg1;
        break;
    case 2:
        g_InvSortRankTable = g_InvSortRankTableD;
        if (arg1 != 0) {
            g_InvSortTypeRankTable = g_InvTypeRankTableA;
        } else {
            g_InvSortTypeRankTable = g_InvTypeRankTableB;
        }
        break;
    }
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    temp_s1 = g_InvItemPtr[g_AyaEquippedWeaponSlot[0]];
    if (g_AyaEquippedArmorSlot[0] >= 0) {
        var_s0 = g_InvItemPtr[g_AyaEquippedArmorSlot[0]];
    } else {
        var_s0 = -1;
    }
    g_InvSortLookup = &Inv_LookupData;
    g_InvSortListBase = g_InvItemPtr;
    g_InvSortListCount = g_InvSlotLimit;
    qsort(g_InvItemPtr, g_InvSlotLimit, 2, &Inv_CompareItemsForSort);
    Inv_SortWeaponSubrange();
    Inv_SortAmmoSubrange();
    for (var_v1 = 0; var_v1 < g_InvSlotLimit; var_v1++) {
        if (g_InvItemPtr[var_v1] == temp_s1) {
            break;
        }
    }
    if (var_v1 < g_InvSlotLimit) {
        g_AyaEquippedWeaponSlot[0] = var_v1;
    }
    if (var_s0 >= 0) {
        for (var_v1_2 = 0; var_v1_2 < g_InvSlotLimit; var_v1_2++) {
            if (g_InvItemPtr[var_v1_2] == var_s0) {
                break;
            }
        }
        if (var_v1_2 < g_InvSlotLimit) {
            g_AyaEquippedArmorSlot[0] = var_v1_2;
        }
    }
    Inv_RebuildSelectableMask();
}

void Inv_SortContainerItems(int arg0) {
    if (arg0 != 0) {
        g_InvSortRankTable = g_InvSortRankTableA;
        g_InvSortTypeRankTable = g_InvTypeRankTableA;
    } else {
        g_InvSortRankTable = g_InvSortRankTableB;
        g_InvSortTypeRankTable = g_InvTypeRankTableB;
    }

    g_InvSortListBase = g_WayneStorageItems;
    g_InvSortListCount = 0x64;
    g_InvSortLookup = Inv_LookupData;
    qsort(g_WayneStorageItems, 0x64, 2, Inv_CompareItemsForSort);
    Inv_SortWeaponSubrange();
    Inv_SortAmmoSubrange();
    Inv_SortInventoryByMode(2, arg0);
    Inv_TransferItemAlt2(0xF400);
}

void Inv_SortInventoryItems(int arg0, int arg1) {
    if (arg0 != 0) {
        g_InvWeaponSortStatIndex = arg1;
        g_InvSortRankTable = g_InvSortRankTableA;
    } else {
        g_InvAmmoSortStatIndex = arg1;
        g_InvSortRankTable = g_InvSortRankTableB;
    }

    g_InvSortListBase = g_InvItemArray;
    g_InvSortTypeRankTable = g_InvTypeRankTableA;
    g_InvSortListCount = 0x52;
    g_InvSortLookup = Inv_LookupData;
    qsort(g_InvItemArray, 0x52, 2, Inv_CompareItemsForSort);
    Inv_SortWeaponSubrange();
    Inv_SortAmmoSubrange();
    Inv_SortInventoryByMode(arg0, arg1);
    Inv_TransferItemAlt2(0x3803FE);
}

void Menu_SetBattleEquipMode(int value) {
    g_MenuBattleEquipMode = value;
}

int Menu_GetBattleEquipMode(void) {
    return g_MenuBattleEquipMode;
}
