/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory.h"
#include "pe1/aya.h"
#include "pe1/battle_cmd.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_state.h"
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

/* Historical name: reload the tracked weapon and return the signed transfer. */
int Inv_DrawSlotItemIcon(void) {
    ItemDataRecord *weapon = 0, *pool;
    unsigned kind;
    int amount, available, loaded;
    int index = D_800C0E20.tracked[0];

    if (index >= 0 && index < D_8009D050)
        weapon = LookupItem(D_8009D048[index]);
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
