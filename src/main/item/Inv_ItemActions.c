/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory_slots.h"
#include "pe1/text.h"
#include "pe1/aya.h"
#include "pe1/menu_state.h"
#include "common.h"
#include "../../../tools/m2c/m2c_macros.h"
#include "pe1/inventory.h"
#include "pe1/battle_cmd.h"
#include "pe1/menu_inventory.h"

/* Item selection masks, ammunition transfer between compared weapons, the
 * weapon comparison panel and the per-slot item actions and active-list controller. Contiguous at
 * 0x80055760 and joined by the comparison records and list state. */

u32 *D_8009D058;
void *Str_LookupTable8(unsigned int index);
void Sfx_DrawSlotRow(ItemDataRecord *entry, u8 *text);
extern u8 g_CursorRenderDataBlock[];

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
    int wasStorage = D_8009D048 != D_800C0E48;
    int i;
    ItemDataRecord *item;
    for (list = 0; list < ListCount(); list++) {
        RestoreList(list);
        for (i = 0; i < D_8009D050; i++)
            if ((0xFE >> Kind(i)) & 1) break;
        hasWeapon |= i < D_8009D050;
        for (i = 0; i < D_8009D050; i++)
            if (Kind(i) == 9) break;
        hasArmor |= i < D_8009D050;
    }
    for (list = 0; list < ListCount(); list++) {
        RestoreList(list);
        ClearBits();
        if (Menu_GetEquipMode()) {
            for (i = 0; i < D_8009D050; i++) {
                if (i >= 0 && i < D_8009D050) item = LookupItem(D_8009D048[i]);
                else item = 0;
                if (item) {
                    int flags = item->flags;
                    u32 selected = g_MenuBattleEquipMode ? ((flags >> 1) & 1) : (flags & 1);
                    if (item->kind == 10 && ConsumableEffect(item) == 2) selected = 0;
                    if ((unsigned)(item->itemId - 6) < 5 && D_800C0E00.current_hp >= D_800C0E00.max_hp) selected = 0;
                    D_8009D058[i >> 5] |= selected << (i & 31);
                }
            }
        } else {
            for (i = 0; i < D_8009D050; i++) {
                if (i >= 0 && i < D_8009D050) item = LookupItem(D_8009D048[i]);
                else item = 0;
                if (item) {
                    int flags = item->flags;
                    u32 selected = g_MenuBattleEquipMode ? ((flags >> 1) & 1) : (flags & 1);
                    if (item->kind == 10) {
                        unsigned effect = ConsumableEffect(item);
                        if ((unsigned)(effect - 4) < 3) selected &= hasWeapon;
                        else if ((unsigned)(effect - 12) < 3) selected &= hasArmor;
                    }
                    if ((unsigned)(item->itemId - 6) < 5 && D_800C0E00.current_hp >= D_800C0E00.max_hp) selected = 0;
                    D_8009D058[i >> 5] |= selected << (i & 31);
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
    for (i = 0; i < D_8009D050; i++) {
        if (i != D_800C0E20.tracked[0] && i != D_800C0E20.tracked[2] && Inv_IsSlotSelectable(i)) {
            if (i >= 0 && i < D_8009D050) item = LookupItem(D_8009D048[i]);
            else item = 0;
            D_8009D058[i >> 5] |= (item ? !(item->flags & 0xE0) : 0) << (i & 31);
        }
    }
}

void Inv_SetSelectionBit(int index) {
    D_8009D058[index >> 5] |= 1u << (index & 31);
}
int Inv_TestSelectionBit(int index) {
    u32 word = D_8009D058[index >> 5];
    u32 mask = 1u << (index & 31);
    return (word & mask) > 0;
}

/* Find a selected alternative, switching lists when necessary. Update the
 * caller's logical list/index pair, then restore the original logical list. */
void Inv_InitSlotDisplay(int *list, int *index) {
    int wasStorage = D_8009D048 != D_800C0E48;
    int i;
    RestoreList(*list);
    for (i = 0; i < D_8009D050; i++)
        if ((D_8009D058[i >> 5] & (1u << (i & 31))) > 0 && i != *index) break;
    if (i < D_8009D050) {
        *index = i;
    } else {
        RestoreList(!*list);
        for (i = 0; i < D_8009D050; i++)
            if ((D_8009D058[i >> 5] & (1u << (i & 31))) > 0) break;
        if (i < D_8009D050) {
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
                    D_8009D058[i >> 5] |= (category == (item->kind && item->kind < 8
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
                    D_8009D058[i >> 5] |= ((unsigned)(candidateKind - 19) < 3 &&
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
                    D_8009D058[i >> 5] |= (category == (item->kind && item->kind < 8
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
                    D_8009D058[i >> 5] |= ((unsigned)(candidateKind - 19) < 3 &&
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

int Spend_Ammo(int amount) {
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
    D_8009D070 = LookupActiveItem(leftIndex);
    RestoreList(rightStorage);
    D_8009D074 = LookupActiveItem(rightIndex);
    g_InvCompareSlotLeft = *D_8009D070;
    g_InvCompareSlotRight = *D_8009D074;
    g_InvCompareSlotRight.tailData[10] = 0;
    g_InvCompareSlotLeft.tailData[10] = 0;
    g_InvCompareSlotRight.reserveAmmo = 0;
    g_InvCompareSlotLeft.reserveAmmo = 0;
}

void Menu_DrawWeaponComparisonPanel(void) {
    u8 *var_a1;
    u8 *var_a1_2;

    if (g_InvCompareSlotLeft.flags & 0x10) {
        var_a1 = g_CursorRenderDataBlock;
        if (g_InvCompareSlotLeft.kind == 9) {
            var_a1 = g_CursorRenderDataBlock + 0x10;
        }
    } else {
        var_a1 = Str_LookupTable8(g_InvCompareSlotLeft.itemId - 1);
    }
    Sfx_DrawSlotRow(&g_InvCompareSlotLeft, var_a1);
    Draw_OffsetCursor(0, 0x18);
    if (g_InvCompareSlotRight.flags & 0x10) {
        var_a1_2 = g_CursorRenderDataBlock;
        if (g_InvCompareSlotRight.kind == 9) {
            var_a1_2 = g_CursorRenderDataBlock + 0x10;
        }
    } else {
        var_a1_2 = Str_LookupTable8(g_InvCompareSlotRight.itemId - 1);
    }
    Sfx_DrawSlotRow(&g_InvCompareSlotRight, var_a1_2);
}

static inline ItemDataRecord *LookupComparisonItem(int value) {
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

static inline ItemDataRecord *LookupComparisonActiveItem(int index) {
    if (index >= 0 && index < D_8009D050) return LookupComparisonItem(D_8009D048[index]);
    return 0;
}

/* Commit comparison snapshots, recording previous ammo for opcode-4 rollback. */
void Inv_StepScrollDisplay(void) {
    BattleCmdEntry *entry;
    ItemDataRecord *pool, *tracked;
    /* Mode 7 ignores the buffer; mode 5 consumes the selected record pointer. */
    int selection[2];
    int mode;
    if (g_InvCompareSlotLeft.ammo == D_8009D070->ammo &&
        g_InvCompareSlotRight.ammo == D_8009D074->ammo) return;
    entry = BattleCmd_AllocSlot();
    entry->header.word = 4;
    entry->payload.ammo_restore.item_data2 = 0;
    entry->payload.ammo_restore.item_data0 = (int)D_8009D070;
    entry->payload.ammo_restore.item_data1 = (int)D_8009D074;
    if (g_InvCompareSlotLeft.reserveAmmo) {
        pool = &D_800A1E44 + g_InvCompareSlotLeft.tailData[10];
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
        pool = &D_800A1E44 + g_InvCompareSlotRight.tailData[10];
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
    entry->payload.ammo_restore.ammo0 = D_8009D070->ammo;
    entry->payload.ammo_restore.ammo1 = D_8009D074->ammo;
    *D_8009D070 = g_InvCompareSlotLeft;
    *D_8009D074 = g_InvCompareSlotRight;
    tracked = LookupComparisonActiveItem(D_800C0E20.tracked[0]);
    if (tracked == D_8009D070) {
        mode = 5;
        selection[0] = (int)tracked;
    } else if (tracked == D_8009D074) {
        selection[0] = (int)tracked;
        mode = 5;
    } else mode = 7;
    Inv_SetActiveList(mode, selection);
}

static inline ItemDataRecord *LookupSlotItem(int value) {
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

/* Historical name: reload the tracked weapon and return the signed transfer. */
int Inv_DrawSlotItemIcon(void) {
    ItemDataRecord *weapon = 0, *pool;
    unsigned kind;
    int amount, available, loaded;
    int index = D_800C0E20.tracked[0];

    if (index >= 0 && index < D_8009D050)
        weapon = LookupSlotItem(D_8009D048[index]);
    kind = weapon->kind;
    if (kind && kind < 8) {
        pool = &D_800A1E64[0];
        if ((int)(kind - 4) > 0)
            pool = &D_800A1E64[(int)(kind - 5)];
    } else if (kind >= 19) {
        pool = &D_800A1E64[(int)(kind - 19)];
    } else {
        pool = &D_800A1E44;
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
        if (D_8009D048 != D_800C0E48 || index != D_800C0E20.tracked[0]) {
            selectable = 1;
        }
    }
    if (data != 0 && data->kind == 8) {
        kind = 8;
        count = 0;
        for (i = 0; i < D_8009D050; i++) {
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
        command->payload.inventory_restore.item_id = D_8009D048[index];
        command->payload.inventory_restore.slot_index = index;
        item = D_8009D048[index];
        Inv_SetActiveList(0,&item);
        Inv_RemoveActiveListItem(index);
    } else {
        switch (((u8 *)data->bonusStats)[0]) {
        case 1:
            BattleCmd_CommitAndSyncAmmo(D_8009D048[index]);
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
    armor = LookupTrackedItem(D_800C0E20.tracked[2]);
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

/* Active-list removal and tracked equipped slots. The two armor aliases
 * preserve independent address reads across the slot write. */
extern s8 g_AyaEquippedWeaponSlot[];
extern s8 g_AyaEquippedArmorSlot[];
extern struct { char _[16]; } D_800C0E22_l1_o __asm__("g_AyaEquippedArmorSlot");
extern struct { char _[16]; } D_800C0E22_s0_o __asm__("g_AyaEquippedArmorSlot");
#define D_800C0E22_l0 (*(s8 *)&D_800C0E22_l1_o)
#define D_800C0E22_l1 (*(s8 *)&D_800C0E22_l1_o)
#define D_800C0E22_s0 (*(s8 *)&D_800C0E22_s0_o)
extern u16 g_BattleCountTable[];
extern int g_MenuBattleCount;

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
    s32 selected;
    s32 removed;
    s32 offset;
    s32 activeList;

    selected = arg0;
    if ((g_InvItemPtr == g_AyaInventoryItems) && (D_800C0E22_l0 == selected)) {
        Inv_GetActiveSlotCount(&sp10);
    }

    activeList = g_InvItemPtr;
    offset = selected << 1;
    slot = (s16 *)(offset + activeList);
    activeList = *slot;
    *slot = 0;
    removed = activeList;

    if (activeList >= 0x100) {
        g_InvItemSlotArray[removed - 0x100].iconId = 0;
    }

    if ((g_InvItemPtr == g_AyaInventoryItems) && (D_800C0E22_l1 == selected)) {
        D_800C0E22_s0 = -1;
        Inv_CheckFreeSlotCapacity(sp10);
        Inv_CompactActiveListSlots();
        Inv_SetActiveList(3, 0);
    }

    return removed;
}


int Inv_LoadWayneItemsAsOverride(short *items) {
    int count = 0;

    if (items != 0) {
        int base = D_8009D03C;
        u16 *out = g_BattleCountTable;
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

        g_InvActiveListOverride = g_BattleCountTable;
        g_InvOverrideSlotLimit = count;
        Inv_RebuildSelectableMask();
    }

    g_MenuBattleCount = count;
    return count;
}
