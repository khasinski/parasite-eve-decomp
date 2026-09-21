/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory_slots.h"
#include "pe1/aya.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_state.h"
#include "pe1/battle_cmd.h"
#include "pe1/menu_queue.h"
#include "pe1/memcard.h"
#include "pe1/game_state.h"

static inline void SelectAya(void) {
    D_8009D048 = D_800C0E00.inventory_items;
    D_8009D050 = Inv_GetAyaSlotLimit();
    D_8009D058 = D_8009D05C;
    D_8009D064 = 2;
}

/* Carry a tagged record forward, removing its first storage-list reference. */
static inline int TakeTagged(int armor) {
    ItemDataRecord *p;
    int id = 0;
    for (p = D_800C0E20.equipment; p < D_800C0E20.equipment + 128; p++) {
        if (p->pad_00[0] && (armor ? p->kind == 9 : p->kind != 9) && (p->flags & 16)) break;
    }
    if (p < D_800C0E20.equipment + 128) {
        s16 *slot;
        id = (p - D_800C0E20.equipment) + 256;
        for (slot = D_800C0E20.specialStorage;
             slot < D_800C0E20.specialStorage + 82; slot++)
            if (*slot == id) break;
        if (slot < D_800C0E20.specialStorage + 82) *slot = 0;
    }
    return id;
}

void Inv_InitNewGameInventory(void) {
    int i, capacity;
    AyaLevelStats *stats;
    u16 *growth = D_800C0E28;
    for (i = 0; i < 7; i++) *growth++ = *(u16 *)Stat_GetGrowthTable(i);
    stats = Aya_LookupLevelStats(0);
    D_800C0E00.max_hp = D_800C0E00.current_hp = stats->hp;
    D_800C0E00.inventory_slot_count = stats->inventoryCapacity;
    D_800C0E00.parasite_spell_flags = 1;
    D_800A1E64[0].ammo = D_800A1E64[1].ammo = D_800A1E64[2].ammo = 0;
    SelectAya();
    capacity = stats->inventoryCapacity;
    if (capacity > 50) capacity = 50;
    D_800C0E00.inventory_slot_count = capacity;
    if (D_8009D048 == D_800C0E00.inventory_items)
        D_8009D050 = Inv_GetAyaSlotLimit();
    SelectAya();
    for (i = 49; i >= 0; i--) D_8009D048[i] = 0;
    Inv_CheckSlotUsable(0x44);
    Inv_CheckSlotUsable(0x96);
    Inv_CheckSlotUsable(0x3F);
    Inv_CheckSlotUsable(1);
    Inv_CheckSlotUsable(6);
    i = TakeTagged(0);
    if (i) Inv_CheckSlotUsable(i);
    i = TakeTagged(1);
    if (i) Inv_CheckSlotUsable(i);
    D_800C0E20.tracked[2] = 1;
    D_800C0E20.tracked[0] = 0;
    D_800C0E00.menu_clamp_value = 61;
    Menu_ClampRange(61);
    D_800C0E00.total_exp = 0;
    D_800C0E00.level = 0;
    Menu_SaveBgInitFade();
}

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


/* Remove the selected record, preserving armor-capacity bookkeeping. */
static inline void RemoveItem(int index) {
    int required, id;
    if (D_8009D048 == D_800C0E48 && D_800C0E20.tracked[2] == index)
        Inv_GetActiveSlotCount(&required);
    id = D_8009D048[index];
    D_8009D048[index] = 0;
    if (id >= 256) D_800C0E20.equipment[id - 256].pad_00[0] = 0;
    if (D_8009D048 == D_800C0E48 && D_800C0E20.tracked[2] == index) {
        D_800C0E20.tracked[2] = -1;
        Inv_CheckFreeSlotCapacity(required);
        Inv_CompactActiveListSlots();
        Inv_SetActiveList(3, 0);
    }
}

static inline int FindKind(int kind, int excluded) {
    /* -1 is an address sentinel; do not form an out-of-bounds C pointer. */
    unsigned long skip = (unsigned long)D_8009D048 + excluded * sizeof(s16);
    s16 *p = D_8009D048;
    while (p < D_8009D048 + D_8009D050) {
        if ((unsigned long)p != skip && LookupItem(*p)->kind == kind) break;
        p++;
    }
    if (p < D_8009D048 + D_8009D050) return p - D_8009D048;
    return -1;
}

void Inv_MergeStorageToSlot(void) {
    int i;
    D_8009D048 = D_800C0E48;
    D_8009D050 = Inv_GetAyaSlotLimit();
    D_8009D058 = D_8009D05C;
    D_8009D064 = 2;
    for (i = 0; i < D_8009D050; i++) {
        unsigned short id = D_8009D048[i];
        if ((unsigned)(id - 256) < 128 &&
            D_800C0E20.equipment[(short)id - 256].itemId == 0x61) break;
    }
    if (i < D_8009D050) RemoveItem(i);
    Inv_CheckSlotUsable(0x93);
    D_800C0E20.tracked[0] = FindKind(7, -1);
    g_MenuBattleEquipMode = 0;
    Inv_SetActiveList(2, 0);
}

/* The caller supplies a fourth script argument; these commands ignore it. */
static inline int FindItem(int id) {
    s16 *p = D_8009D048;
    while (p < D_8009D048 + D_8009D050) {
        if (*p == id) break;
        p++;
    }
    if (p < D_8009D048 + D_8009D050) return p - D_8009D048;
    return -1;
}

static inline void SetCapacity(int value) {
    if (value > 50) value = 50;
    D_800C0E00.inventory_slot_count = value;
    if (D_8009D048 == D_800C0E48) D_8009D050 = Inv_GetAyaSlotLimit();
}

int Menu_InitBonusPointScreen(int command, int value, int other, int *unused) {
    switch (command) {
    case 1100: {
        s16 *p;
        int count = 0;
        SelectAya();
        for (p = D_8009D048; p < D_8009D048 + D_8009D050; p++)
            count += (*p != 0);
        return count;
    }
    case 1101: {
        int bonus = Inv_GetBonusSlotCount();
        if (g_InvBaseCapacityForLimit[0] + bonus < 51) {
            bonus = Inv_GetBonusSlotCount();
            return g_InvBaseCapacityForResult[0] + bonus;
        }
        return 50;
    }
    case 1102: {
        int count = 0;
        s16 *p;
        if (value >= D_8009D03C && value < D_8009D03C + 3)
            count = D_800A1E64[value - D_8009D03C].ammo;
        else for (p = D_8009D048; p < D_8009D048 + D_8009D050; p++) {
            unsigned short id = *p;
            if ((unsigned)(id - 256) < 128)
                count += (D_800C0E20.equipment[(short)id - 256].itemId == value);
            else
                count += ((short)id == value);
        }
        return count;
    }
    case 1103:
        D_8009D0CC = value;
        D_8009D0D0 = other;
        return 0;
    case 1104:
        return Inv_RebuildWithBonusSlots(value, other);
    case 1105:
        SetCapacity(value);
        return 0;
    case 1106:
        return D_800C0E00.current_hp;
    case 1107:
        return D_800C0E00.max_hp;
    case 1108:
        BattleCmd_SetCurrentHP(value);
        return 0;
    case 1109:
        return BattleCmd_GetRemainingAmmo(0);
    case 1110: {
        int result;
        BattleCmd_GetRemainingAmmo(&result);
        return result;
    }
    case 1111:
        BattleCmd_SetCurrentMP(value);
        return 0;
    case 1112: {
        int index;
        SelectAya();
        index = FindItem(value);
        if (index < 0) return index;
        Inv_RemoveActiveListItem(index);
        return 0;
    }
    case 1113:
        Inv_TransferToStorage();
        Queue_Init();
        Menu_StepInventoryRoot(0, -3, -1);
        Menu_CreateContextHelpPanel();
        return 0;
    case 1114:
        Menu_ComputeGammaLut(value, other);
        return 0;
    case 1115:
        MemCard_InitSlotState();
        return 0;
    case 1116:
        Menu_SaveBgStartFadeOut();
        return 0;
    case 1117:
        Inv_RebuildSelectionBitset();
        g_GameState.pending_story_day = 1;
        Menu_SetMemCardConfirmPending();
        if (D_800C0E00.pad_0B + 1 < 100) D_800C0E00.pad_0B++;
        else D_800C0E00.pad_0B = 99;
        Inv_InitNewGameInventory();
        return 0;
    case 1118:
        Menu_InitBonusPointAllocState(value);
        return 0;
    case 1119: {
        s16 *p;
        int missing;
        for (p = D_800C0E20.storage; p < D_800C0E20.storage + 100; p++)
            if (*p == value) break;
        missing = !(p < D_800C0E20.storage + 100);
        if (!missing) *p = 0;
        return missing;
    }
    case 1120:
        Inv_MergeStorageToSlot();
    }
    return 0;
}
