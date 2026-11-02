/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/aya.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_modifiers.h"
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory_state.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_state.h"
#include "pe1/random.h"
#include "pe1/text.h"
#include "m2c_macros.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
s16 *g_InvItemPtr;
s32 g_InvSlotLimit;
u32 *g_InvSelectionBits;
s32 g_InvSelectionBitWords;
int g_InvCategoryBaseItemId;
s32 g_ItemDropRngIndex;
int g_InvPackedListCursor;
int g_InvPackedListCount;
short g_InvPackedList[];

static inline int RandomByte(void) {
    int i;
    if (++g_ItemDropRngIndex >= 521) {
        for (i = 0; i < 32; ++i)
            g_ItemDropRngTable[i] ^= g_ItemDropRngTable[i + 489];
        for (i = 32; i < 521; ++i)
            g_ItemDropRngTable[i] ^= g_ItemDropRngTable[i - 32];
        g_ItemDropRngIndex = 0;
    }
    return g_ItemDropRngTable[g_ItemDropRngIndex];
}
void Inv_RollRandomItemType(s16 *type, s16 *amount) {
    /* Retail builds this list but only uses its count when choosing a type. */
    int available[3];
    int count;
    int i;
    u16 *stock;
    if (type && amount) {
        if (!*type) {
            count = 0;
            for (i = 0; i < 3; ++i)
                if (g_InvSpecialItemRecords[i].ammo) available[count++] = i;
            if (count) *type = ((count * RandomByte()) >> 8) + 1;
        }
        {
            u16 chosen = *type;
            if (chosen - 1U < 3U) {
                stock = &g_InvSpecialItemRecords[(s16)chosen - 1].ammo;
                if (!*amount) *amount = (*stock * RandomByte()) >> 8;
                else if (*amount < 0) *amount = (*stock * -*amount) / 100;
                if (*amount > *stock) *amount = *stock;
                *stock -= *amount;
            }
        }
    }
}

void Item_PackParasiteSpells(void) {
    int flags;
    short *out;
    int i;

    flags = g_AyaParasiteSpellFlags[0];
    out = g_InvPackedList;

    for (i = 0; i < 0x14; i++) {
        if (flags & 1) {
            *out++ = i;
        }
        flags >>= 1;
    }

    g_InvPackedListCursor = 0;
    g_InvPackedListCount = out - g_InvPackedList;
}

/* Learns the parasite spell whose unlock key matches and announces it.
 * Returns whether the key named one of the 20 spells. */
int Aya_UnlockParasiteSpellById(int key)
{
    ParasiteSpellEntry *table;
    int spell;
    int found;

    table = Aya_GetParasiteSpellUnlockTable();
    for (spell = 0; spell < 20; spell++) {
        if (table[spell].unlockKey == key) {
            break;
        }
    }
    found = spell < 20;
    if (found) {
        g_AyaParasiteSpellFlags[0] |= 1 << spell;
        Menu_CreateParasiteSpellInfoPanel(spell);
    }
    return found;
}

int Inv_GetPackedListItem(int arg0) {
    if (arg0 >= 0 && arg0 < g_InvPackedListCount) {
        return g_InvPackedList[arg0];
    }
    return 0;
}

void Inv_ClearSelectionBitset(void) {
    int i;

    for (i = 0; i < g_InvSelectionBitWords; i++) {
        g_InvSelectionBits[i] = 0;
    }
}

/* Item selection masks, ammunition transfer between compared weapons, the
 * weapon comparison panel and the per-slot item actions and active-list controller. Contiguous at
 * 0x80055760 and joined by the comparison records and list state. */

ItemDataRecord *g_InvCompareLeftItem, *g_InvCompareRightItem;
s16 *g_InvActiveListOverride;
s32 g_InvOverrideSlotLimit;
int g_InvActiveListCount;
int g_MenuBattleEquipMode;
void Menu_DrawSlotRow(ItemDataRecord *entry, u8 *text);
int Inv_CheckFreeSlotCapacity(int requested_slots);

static inline ItemDataRecord *LookupActiveItem(int index) {
    if (index >= 0 && index < g_InvSlotLimit) return Inv_LookupItemRecordScaled(g_InvItemPtr[index]);
    return 0;
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

static inline int ListCount(void) {
    if (g_InvActiveListOverride) return 2;
    return 1;
}

static inline void ClearBits(void) {
    int i;
    for (i = 0; i < g_InvSelectionBitWords; i++) g_InvSelectionBits[i] = 0;
}

static inline int CountBits(void) {
    int i, count = 0;
    for (i = 0; i < g_InvSlotLimit; i++)
        count += (g_InvSelectionBits[i >> 5] & (1u << (i & 31))) > 0;
    return count;
}

static inline int Kind(int index) {
    ItemDataRecord *item = LookupActiveItem(index);
    return item ? item->kind : 0;
}
static inline u8 ConsumableEffect(ItemDataRecord *item) {
    return ((u8 *)item->bonusStats)[0];
}
/* First discover available equipment, then rebuild masks for the current menu.
 * Resolved record kinds must be below 32 for the weapon-kind variable shift. */
void Inv_RebuildSelectableMask(void) {
    int hasWeapon = 0, hasArmor = 0, list = 0;
    int wasStorage = g_InvItemPtr != g_AyaInventoryItems;
    int i;
    ItemDataRecord *item;
    for (list = 0; list < ListCount(); list++) {
        RestoreList(list);
        for (i = 0; i < g_InvSlotLimit; i++)
            if ((0xFE >> Kind(i)) & 1) break;
        hasWeapon |= i < g_InvSlotLimit;
        for (i = 0; i < g_InvSlotLimit; i++)
            if (Kind(i) == 9) break;
        hasArmor |= i < g_InvSlotLimit;
    }
    for (list = 0; list < ListCount(); list++) {
        RestoreList(list);
        ClearBits();
        if (Menu_GetEquipMode()) {
            for (i = 0; i < g_InvSlotLimit; i++) {
                if (i >= 0 && i < g_InvSlotLimit) item = Inv_LookupItemRecordScaled(g_InvItemPtr[i]);
                else item = 0;
                if (item) {
                    int flags = item->flags;
                    u32 selected = g_MenuBattleEquipMode ? ((flags >> 1) & 1) : (flags & 1);
                    if (item->kind == 10 && ConsumableEffect(item) == 2) selected = 0;
                    if (item->itemId - 6U < 5U && g_AyaSaveState.current_hp >= g_AyaSaveState.max_hp) selected = 0;
                    g_InvSelectionBits[i >> 5] |= selected << (i & 31);
                }
            }
        } else {
            for (i = 0; i < g_InvSlotLimit; i++) {
                if (i >= 0 && i < g_InvSlotLimit) item = Inv_LookupItemRecordScaled(g_InvItemPtr[i]);
                else item = 0;
                if (item) {
                    int flags = item->flags;
                    u32 selected = g_MenuBattleEquipMode ? ((flags >> 1) & 1) : (flags & 1);
                    if (item->kind == 10) {
                        unsigned effect = ConsumableEffect(item);
                        if (effect - 4U < 3U) selected &= hasWeapon;
                        else if (effect - 12U < 3U) selected &= hasArmor;
                    }
                    if (item->itemId - 6U < 5U && g_AyaSaveState.current_hp >= g_AyaSaveState.max_hp) selected = 0;
                    g_InvSelectionBits[i >> 5] |= selected << (i & 31);
                }
            }
        }
    }
    RestoreList(wasStorage);
}

/* Historical name: rebuild selectable slots, excluding tracked equipment
 * and records whose high three flag bits are set. */
void Inv_SortSlotsByPriority(void) {
    int i;
    ItemDataRecord *item;
    ClearBits();
    for (i = 0; i < g_InvSlotLimit; i++) {
        if (i != g_InventoryRuntime.tracked[0] && i != g_InventoryRuntime.tracked[2] && Inv_IsSlotSelectable(i)) {
            if (i >= 0 && i < g_InvSlotLimit) item = Inv_LookupItemRecordScaled(g_InvItemPtr[i]);
            else item = 0;
            g_InvSelectionBits[i >> 5] |= (item ? !(item->flags & 0xE0) : 0) << (i & 31);
        }
    }
}

void Inv_SetSelectionBit(int index) {
    g_InvSelectionBits[index >> 5] |= 1u << (index & 31);
}
int Inv_TestSelectionBit(int index) {
    u32 word = g_InvSelectionBits[index >> 5];
    u32 mask = 1u << (index & 31);
    return (word & mask) > 0;
}

/* Find a selected alternative, switching lists when necessary. Update the
 * caller's logical list/index pair, then restore the original logical list. */
void Inv_InitSlotDisplay(int *list, int *index) {
    int wasStorage = g_InvItemPtr != g_AyaInventoryItems;
    int i;
    RestoreList(*list);
    for (i = 0; i < g_InvSlotLimit; i++)
        if ((g_InvSelectionBits[i >> 5] & (1u << (i & 31))) > 0 && i != *index) break;
    if (i < g_InvSlotLimit) {
        *index = i;
    } else {
        RestoreList(!*list);
        for (i = 0; i < g_InvSlotLimit; i++)
            if ((g_InvSelectionBits[i >> 5] & (1u << (i & 31))) > 0) break;
        if (i < g_InvSlotLimit) {
            *list = !*list;
            *index = i;
        } else *list = -1;
    }
    RestoreList(wasStorage);
}

/* Build ammunition-compatible selections in the current list and, when present,
 * the other list. Exclude sourceIndex only in the original list. The source
 * slot must resolve to an item. The return value counts both selections. */
int Inv_BuildCompatibleWeaponBitset(int sourceIndex) {
    ItemDataRecord *item;
    unsigned kind;
    int category, i, count, wasStorage;
    if (sourceIndex >= 0 && sourceIndex < g_InvSlotLimit) item = Inv_LookupItemRecordScaled(g_InvItemPtr[sourceIndex]);
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
    if (kind - 19U < 3U) {
        for (i = 0; i < g_InvSlotLimit; i++) {
            if (i != sourceIndex) {
                if (i >= 0 && i < g_InvSlotLimit) item = Inv_LookupItemRecordScaled(g_InvItemPtr[i]);
                else item = 0;
                if (item) {
                    g_InvSelectionBits[i >> 5] |= (category == (item->kind && item->kind < 8
                        ? ((int)item->kind - 4 > 0 ? (int)item->kind - 4 : 1)
                        : item->kind >= 19 ? item->kind - 18 : 0)) << (i & 31);
                }
            }
        }
    } else {
        for (i = 0; i < g_InvSlotLimit; i++) {
            if (i != sourceIndex) {
                if (i >= 0 && i < g_InvSlotLimit) item = Inv_LookupItemRecordScaled(g_InvItemPtr[i]);
                else item = 0;
                if (item) {
                    u16 candidateKind = item->kind;
                    g_InvSelectionBits[i >> 5] |= (candidateKind - 19U < 3U &&
                        category == (item->kind && item->kind < 8
                        ? ((int)item->kind - 4 > 0 ? (int)item->kind - 4 : 1)
                        : item->kind >= 19 ? item->kind - 18 : 0)) << (i & 31);
                }
            }
        }
    }
    count = CountBits();
    wasStorage = g_InvItemPtr != g_AyaInventoryItems;
    if (wasStorage || ListCount() == 2) {
        RestoreList(!wasStorage);
        ClearBits();
        if (kind - 19U < 3U) {
            for (i = 0; i < g_InvSlotLimit; i++) {
                if (i >= 0 && i < g_InvSlotLimit) item = Inv_LookupItemRecordScaled(g_InvItemPtr[i]);
                else item = 0;
                if (item) {
                    g_InvSelectionBits[i >> 5] |= (category == (item->kind && item->kind < 8
                        ? ((int)item->kind - 4 > 0 ? (int)item->kind - 4 : 1)
                        : item->kind >= 19 ? item->kind - 18 : 0)) << (i & 31);
                }
            }
        } else {
            for (i = 0; i < g_InvSlotLimit; i++) {
                if (i >= 0 && i < g_InvSlotLimit) item = Inv_LookupItemRecordScaled(g_InvItemPtr[i]);
                else item = 0;
                if (item) {
                    u16 candidateKind = item->kind;
                    g_InvSelectionBits[i >> 5] |= (candidateKind - 19U < 3U &&
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

int Inv_SpendAmmo(int amount) {
    InvItemSlot *src;
    InvItemSlot *dst;
    int new_src;
    int new_dst;
    int max;
    int ret;

    ret = 0;
    if (amount > 0) {
        src = &g_InvCompareSlotLeft;
        dst = &g_InvCompareSlotRight;
    } else {
        src = &g_InvCompareSlotRight;
        dst = &g_InvCompareSlotLeft;
        amount = -amount;
    }

    if (src != 0 && dst != 0) {
        new_src = src->ammo - amount;
        new_dst = dst->ammo + amount;
        if (new_src < 0) {
            new_dst += new_src;
            new_src = 0;
            ret = 1;
        }

        max = dst->baseStats[2] + dst->bonusStats[2];
        if (max >= 1000) {
            max = 999;
        }
        if (max < new_dst) {
            max = dst->baseStats[2] + dst->bonusStats[2];
            if (max >= 1000) {
                max = 999;
            }
            new_src += new_dst - max;
            new_dst = dst->baseStats[2] + dst->bonusStats[2];
            ret = 2;
            if (new_dst >= 1000) {
                new_dst = 999;
            }
        }

        src->ammo = new_src;
        dst->ammo = new_dst;
        if (new_src == 0 && src->reserveAmmo != 0) {
            src->ammo = src->reserveAmmo;
            src->reserveAmmo = 0;
        }
    } else {
        ret = 3;
    }

    return ret;
}

int Inv_GetWeaponCategoryAmmoBase(unsigned int arg0) {
    if (arg0 >= 3) {
        return 0;
    }
    return g_InvCategoryItemTable[arg0].count;
}

/* Keep resolved pointers for the comparison UI, then snapshot both records.
 * Both retained pointers must be valid at copy time. Copies are sequential,
 * including when aliased. */
void Inv_BuildDisplayFromList(int leftStorage, int leftIndex, int rightStorage, int rightIndex) {
    RestoreList(leftStorage);
    g_InvCompareLeftItem = LookupActiveItem(leftIndex);
    RestoreList(rightStorage);
    g_InvCompareRightItem = LookupActiveItem(rightIndex);
    g_InvCompareSlotLeft = *g_InvCompareLeftItem;
    g_InvCompareSlotRight = *g_InvCompareRightItem;
    g_InvCompareSlotRight.tailData[10] = 0;
    g_InvCompareSlotLeft.tailData[10] = 0;
    g_InvCompareSlotRight.reserveAmmo = 0;
    g_InvCompareSlotLeft.reserveAmmo = 0;
}

void Menu_DrawWeaponComparisonPanel(void) {
    u8 *var_a1;
    u8 *var_a1_2;

    if (g_InvCompareSlotLeft.flags & 0x10) {
        var_a1 = g_CursorRenderMetadataWindows[0].text;
        if (g_InvCompareSlotLeft.kind == 9) {
            var_a1 = g_CursorRenderMetadataWindows[1].text;
        }
    } else {
        var_a1 = Text_LookupTable8(g_InvCompareSlotLeft.itemId - 1);
    }
    Menu_DrawSlotRow(&g_InvCompareSlotLeft, var_a1);
    Draw_OffsetCursor(0, 0x18);
    if (g_InvCompareSlotRight.flags & 0x10) {
        var_a1_2 = g_CursorRenderMetadataWindows[0].text;
        if (g_InvCompareSlotRight.kind == 9) {
            var_a1_2 = g_CursorRenderMetadataWindows[1].text;
        }
    } else {
        var_a1_2 = Text_LookupTable8(g_InvCompareSlotRight.itemId - 1);
    }
    Menu_DrawSlotRow(&g_InvCompareSlotRight, var_a1_2);
}

static inline ItemDataRecord *LookupComparisonItem(int value) {
    int saved = value;
    ItemDataRecord *result;
    if (value - 0x100U < 0x80U) {
        result = &g_InventoryRuntime.equipment[value - 0x100];
    } else {
        if (value - 1U < 0xFFU) {
            result = Item_LookupBaseData(value - 1);
        } else if (saved - 0x200U < 9U) {
            int shifted = saved << 5;
            result = (ItemDataRecord *)(g_KeyItemDataTable + shifted);
        } else {
            result = 0;
        }
    }
    return result;
}

static inline ItemDataRecord *LookupComparisonActiveItem(int index) {
    if (index >= 0 && index < g_InvSlotLimit) return LookupComparisonItem(g_InvItemPtr[index]);
    return 0;
}

/* Commit comparison snapshots, recording previous ammo for opcode-4 rollback. */
void Inv_StepScrollDisplay(void) {
    BattleCmdEntry *entry;
    ItemDataRecord *pool, *tracked;
    /* Mode 7 ignores the buffer; mode 5 consumes the selected record pointer. */
    int selection[2];
    int mode;
    if (g_InvCompareSlotLeft.ammo == g_InvCompareLeftItem->ammo &&
        g_InvCompareSlotRight.ammo == g_InvCompareRightItem->ammo) return;
    entry = BattleCmd_AllocSlot();
    entry->header.word = 4;
    entry->payload.ammo_restore.item_data2 = 0;
    entry->payload.ammo_restore.item_data0 = (int)g_InvCompareLeftItem;
    entry->payload.ammo_restore.item_data1 = (int)g_InvCompareRightItem;
    if (g_InvCompareSlotLeft.reserveAmmo) {
        pool = &g_InvFallbackAmmoRecord + g_InvCompareSlotLeft.tailData[10];
        entry->payload.ammo_restore.item_data2 = (int)pool;
        if (pool) {
            entry->payload.ammo_restore.ammo2 = pool->ammo;
            {
                u16 result;
                int total = pool->ammo + g_InvCompareSlotLeft.reserveAmmo;
                int capacity = pool->baseStats[2] + pool->bonusStats[2];
                if (capacity < 1000 ? capacity < total : 999 < total) {
                    result = pool->baseStats[2] + pool->bonusStats[2] < 1000 ?
                        pool->baseStats[2] + (u16)pool->bonusStats[2] : 999;
                } else {
                    result = pool->ammo + g_InvCompareSlotLeft.reserveAmmo;
                }
                pool->ammo = result;
            }
        }
    } else if (g_InvCompareSlotRight.reserveAmmo) {
        pool = &g_InvFallbackAmmoRecord + g_InvCompareSlotRight.tailData[10];
        entry->payload.ammo_restore.item_data2 = (int)pool;
        if (pool) {
            entry->payload.ammo_restore.ammo2 = pool->ammo;
            {
                u16 result;
                int total = pool->ammo + g_InvCompareSlotRight.reserveAmmo;
                int capacity = pool->baseStats[2] + pool->bonusStats[2];
                if (capacity < 1000 ? capacity < total : 999 < total) {
                    result = pool->baseStats[2] + pool->bonusStats[2] < 1000 ?
                        pool->baseStats[2] + (u16)pool->bonusStats[2] : 999;
                } else {
                    result = pool->ammo + g_InvCompareSlotRight.reserveAmmo;
                }
                pool->ammo = result;
            }
        }
    }
    entry->payload.ammo_restore.ammo0 = g_InvCompareLeftItem->ammo;
    entry->payload.ammo_restore.ammo1 = g_InvCompareRightItem->ammo;
    *g_InvCompareLeftItem = g_InvCompareSlotLeft;
    *g_InvCompareRightItem = g_InvCompareSlotRight;
    tracked = LookupComparisonActiveItem(g_InventoryRuntime.tracked[0]);
    if (tracked == g_InvCompareLeftItem) {
        mode = 5;
        selection[0] = (int)tracked;
    } else if (tracked == g_InvCompareRightItem) {
        selection[0] = (int)tracked;
        mode = 5;
    } else mode = 7;
    Inv_SetActiveList(mode, selection);
}

static inline ItemDataRecord *LookupSlotItem(int value) {
    int saved = value;
    ItemDataRecord *result;
    if (value - 0x100U < 0x80U) {
        result = &g_InventoryRuntime.equipment[value - 0x100];
    } else {
        if (value - 1U < 0xFFU) {
            result = Item_LookupBaseData(value - 1);
        } else if (saved - 0x200U < 9U) {
            result = &g_InvSpecialItemRecords[saved - 512];
        } else {
            result = 0;
        }
    }
    return result;
}

/* Historical name: reload the tracked weapon and return the signed transfer. */
int Inv_DrawSlotItemIcon(void) {
    ItemDataRecord *weapon = 0, *pool;
    unsigned kind;
    int amount, available, loaded;
    int index = g_InventoryRuntime.tracked[0];

    if (index >= 0 && index < g_InvSlotLimit)
        weapon = LookupSlotItem(g_InvItemPtr[index]);
    kind = weapon->kind;
    if (kind && kind < 8) {
        pool = &g_InvSpecialItemRecords[0];
        if ((int)(kind - 4) > 0)
            pool = &g_InvSpecialItemRecords[(int)(kind - 5)];
    } else if (kind >= 19) {
        pool = &g_InvSpecialItemRecords[(int)(kind - 19)];
    } else {
        pool = &g_InvFallbackAmmoRecord;
    }
    amount = weapon->baseStats[2] + weapon->bonusStats[2];
    available = pool->ammo;
    loaded = weapon->ammo;
    if (amount < 1000 ? amount - loaded < available : 999 - loaded < available) {
        int current, capacity;
        capacity = weapon->baseStats[2] + weapon->bonusStats[2];
        current = weapon->ammo;
        amount = capacity < 1000 ? capacity - current : 999 - current;
    } else {
        amount = pool->ammo;
    }
    pool->ammo -= amount;
    weapon->ammo += amount;
    return amount;
}

static inline ItemDataRecord *LookupTrackedItem(int index) {
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
                int shifted = saved << 5;
                result = (ItemDataRecord *)(g_KeyItemDataTable + shifted);
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

int Inv_IsSlotSelectable(int index) {
    ItemDataRecord *data;
    int selectable, count, i;
    int kind;
    data = LookupTrackedItem(index);
    if (data == 0) {
        return 1;
    }
    selectable = 0;
    if (!(data->flags & ITEM_DATA_FLAG_DISABLED)) {
        if (g_InvItemPtr != g_AyaInventoryItems || index != g_InventoryRuntime.tracked[0]) {
            selectable = 1;
        }
    }
    if (data != 0 && data->kind == 8) {
        kind = 8;
        count = 0;
        for (i = 0; i < g_InvSlotLimit; i++) {
            if (Inv_GetActiveListItemType(i) == kind) {
                count++;
            }
        }
        selectable &= count >= 2;
    }
    return selectable;
}

void Item_SetDisabledFlag(int arg0, int arg1) {
    ItemDataRecord *entry;
    int flags;

    entry = Item_LookupBaseData(arg0 - 1);
    if (entry != 0) {
        flags = entry->flags & (u8)~ITEM_DATA_FLAG_DISABLED;
        entry->flags = flags;
        if (arg1 == 0) {
            flags |= ITEM_DATA_FLAG_DISABLED;
        }
        entry->flags = flags;
    }
}

void Inv_BuildEquipSlotDisplay(int index) {
    ItemDataRecord *data;
    BattleCmdEntry *command;
    int item;
    data = LookupTrackedItem(index);
    if (g_MenuBattleEquipMode) {
        command = BattleCmd_AllocSlot();
        command->header.word = 0;
        command->payload.inventory_restore.item_id = g_InvItemPtr[index];
        command->payload.inventory_restore.slot_index = index;
        item = g_InvItemPtr[index];
        Inv_SetActiveList(0,&item);
        Inv_RemoveActiveListItem(index);
    } else {
        switch (((u8 *)data->bonusStats)[0]) {
        case 1:
            BattleCmd_CommitAndSyncAmmo(g_InvItemPtr[index]);
            Inv_RemoveActiveListItem(index);
            Inv_RebuildSelectableMask();
            break;
        case 2:
            Menu_OpenSkillSelectionView();
            break;
        case 4: case 5: case 6:
            Menu_StepInventoryRoot(0xFE,index,-1);
            break;
        case 12: case 13: case 14:
            Menu_StepInventoryRoot(0x200,index,-1);
            break;
        }
    }
}

int Inv_GetSlotHighlightState(int spell, int available) {
    int remaining, cost, i;
    ItemDataRecord *armor;
    ParasiteSpellEntry *table;
    if (spell == 6 || spell == 19) {
        return available;
    }
    if (spell == 5) {
        BattleCmd_GetRemainingAmmo(&remaining);
        return remaining / 3;
    }
    table = Aya_GetParasiteSpellUnlockTable();
    cost = table[spell].cost;
    armor = LookupTrackedItem(g_InventoryRuntime.tracked[2]);
    if (armor != 0) {
        for (i = 0; i < armor->tailCount; i++) {
            if (armor->tailData[i] == 0x0E) {
                break;
            }
        }
        if (i < armor->tailCount) {
            cost = cost * 2 / 3;
        }
    }
    return cost;
}

void Battle_UseItem(s32 arg0) {
    s32 sp10;
    s32 temp_s0;
    ItemDataRecord *temp_v0;
    BattleCmdEntry *temp_v0_2;

    temp_v0 = Item_LookupBaseData(arg0 + 0xEB);
    if (g_MenuBattleEquipMode != 0) {
        temp_s0 = Inv_GetSlotHighlightState(arg0, BattleCmd_GetRemainingAmmo(0));
        temp_v0_2 = BattleCmd_AllocSlot();
        temp_v0_2->header.word = 1;
        temp_v0_2->payload.ammo_spend.item_index = arg0;
        temp_v0_2->payload.ammo_spend.amount = temp_s0;
        sp10 = arg0;
        Inv_SetActiveList(1, &sp10);
        return;
    }
    if (*(u8 *)&temp_v0->bonusStats[0] == 1) {
        BattleCmd_ChangeWeaponAndSync(arg0);
    }
}

void Inv_DropCurrentSelectionItem(void) {
    MenuWidgetNode *node = MenuWidget_FindByModeAndSelectedBase(2, 1);
    int index;

    if (node != 0) {
        index = MenuWidget_GridCellIndex(node);
        if (index >= 0) {
            Inv_RemoveActiveListItem(index);
        }
    }
}

int Inv_SwapSlots(int unused, int from, int unused2, int to) {
    g_InvItemPtr[from] ^= g_InvItemPtr[to];
    g_InvItemPtr[to] ^= g_InvItemPtr[from];
    g_InvItemPtr[from] ^= g_InvItemPtr[to];

    if (g_AyaEquippedWeaponSlot[0] == from) {
        g_AyaEquippedWeaponSlot[0] = to;
    } else if (g_AyaEquippedWeaponSlot[0] == to) {
        g_AyaEquippedWeaponSlot[0] = from;
    }

    if (g_AyaEquippedArmorSlot[0] == from) {
        g_AyaEquippedArmorSlot[0] = to;
    } else if (g_AyaEquippedArmorSlot[0] == to) {
        g_AyaEquippedArmorSlot[0] = from;
    }

    Inv_RebuildSelectableMask();
    return 1;
}

int Inv_ClearActiveListSlot(int arg0) {
    int value;

    value = g_InvItemPtr[arg0];
    g_InvItemPtr[arg0] = 0;
    return value;
}

s32 Inv_RemoveActiveListItem(s32 arg0) {
    s32 sp10;
    s16 *slot;
    s32 removed;
    s32 offset;
    s32 activeList;

    if ((g_InvItemPtr == g_AyaInventoryItems) && (g_EquippedArmorSlot == arg0)) {
        Inv_GetActiveSlotCount(&sp10);
        Inv_CheckFreeSlotCapacity(sp10);
        g_EquippedArmorSlot = -1;
        Inv_SetActiveList(3, 0);
    }

    activeList = g_InvItemPtr;
    offset = arg0 << 1;
    slot = (s16 *)(offset + activeList);
    removed = *slot;
    *slot = 0;

    if (removed >= 0x100) {
        g_InvItemSlotArray[removed - 0x100].iconId = 0;
    }

    return removed;
}

int Inv_LoadWayneItemsAsOverride(short *items) {
    int count = 0;

    if (items != 0) {
        int base = g_InvCategoryBaseItemId;
        u16 *out = g_InvActiveListItems;
        int end = base + 3;
        int id;
        while (count < 10 && (id = items[0]) != 0) {
            if ((base <= id) && (id < end)) {
                int temp = id + 6;

                id = temp - base;
                temp = id + 0x200;
                *out = temp;
                g_InvCategoryItemTable[id].count = (u16)items[1];
                out++;
            } else {
                *out = id;
                out++;
            }

            count++;
            items += 2;
        }

        g_InvActiveListOverride = g_InvActiveListItems;
        g_InvOverrideSlotLimit = count;
        Inv_RebuildSelectableMask();
    }

    g_InvActiveListCount = count;
    return count;
}
