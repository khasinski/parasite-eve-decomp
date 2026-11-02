/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/aya.h"
#include "pe1/battle.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_modifiers.h"
#include "pe1/battle_runtime.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/player_entity.h"
#include "pe1/weapon_list_output.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
BattleCmdEntry *g_BattleCmdStackTop;
u8 *g_MenuActiveListTarget;
int g_MenuCommandResult;
int g_InvBonusSlotCount;

void BattleCmd_InitTableCursor(void) {
    g_BattleCmdStackTop = g_BattleCmdStack;
}

BattleCmdEntry *BattleCmd_AllocSlot(void) {
    BattleCmdEntry *top;
    BattleCmdEntry *end;
    BattleCmdEntry *limit;
    BattleCmdEntry *next;

    top = g_BattleCmdStackTop;
    end = (BattleCmdEntry *)g_StatScaleBase;
    if (top < end) {
        next = top + 1;
        g_BattleCmdStackTop = next;
    } else {
        top = end - 4;
        end = end - 1;
        if (top < end) {
            limit = end;
            do {
                BattleCmdEntry *src = top + 1;
                *top = *src;
                top += 1;
            } while (top < limit);
        }
    }
    return top;
}

void BattleCmd_UndoPending(void) {
    BattleCmdEntry *entry;
    BattleCmdEntry *entry_tmp;
    BattleCmdEntry *top;
    int opcode;
    s16 *slot;
    ItemDataRecord *data;
    int index;

    top = g_BattleCmdStackTop;
    if (g_BattleCmdStack < top) {
        entry_tmp = top - 1;
        opcode = (top - 1)->header.word;
        g_BattleCmdStackTop = entry_tmp;
        entry = entry_tmp;

        switch (opcode) {
        case 0:
            slot = &g_AyaInventoryItems[entry->payload.inventory_restore.slot_index];
            if (*slot == 0) {
                *slot = entry->payload.inventory_restore.item_id;
            } else {
                Inv_AddItem(entry->payload.inventory_restore.item_id);
            }
            break;

        case 1:
            break;

        case 2:
            g_InvTrackedSlots[0] =
                Inv_FindIndexByData(entry->payload.equip_restore.item_data);
            break;

        case 3:
            data = entry->payload.equip_restore.item_data;
            index = -1;
            if (data != 0) {
                index = Inv_FindIndexByData(data);
            }
            g_AyaEquippedArmorSlot[0] = index;
            break;

        case 4:
            ((ItemDataRecord *)entry->payload.ammo_restore.item_data0)->ammo =
                entry->payload.ammo_restore.ammo0;
            ((ItemDataRecord *)entry->payload.ammo_restore.item_data1)->ammo =
                entry->payload.ammo_restore.ammo1;
            ((ItemDataRecord *)entry->payload.ammo_restore.item_data2)->ammo =
                entry->payload.ammo_restore.ammo2;
            break;
        }
    }
}

void BattleCmd_ResetTableCursor(void) {
    g_BattleCmdStackTop = g_BattleCmdStack;
}

void BattleCmd_UndoAllPending(void) {
    if (g_BattleCmdStack < g_BattleCmdStackTop) {
        BattleCmdEntry *base = g_BattleCmdStack;

        do {
            BattleCmd_UndoPending();
        } while (base < g_BattleCmdStackTop);
    }
}

/* Active-list commands from the item menus and the pending command result
 * (g_MenuCommandResult) that the menu loop polls and clears. */

void Battle_DispatchSpecialAction(int);

void Inv_SetActiveList(int mode, int *slot) {
    Combatant *active;
    int *selection;
    BattleEntity *entity;
    int value;
    int present;
    selection = slot;
    entity = D_8009D254;
    active = 0;
    if (entity) {
        value = (int)entity->core;
        present = value != 0;
            active = (Combatant *)(value & -present);
    }
    switch (mode) {
    case 0:
        g_MenuCommandResult = *selection + 3;
        break;
    case 1:
        g_MenuCommandResult = *selection + 0x183;
        break;
    case 2:
        if (active) {
            u8 *out = g_MenuActiveListTarget;
            if (out) {
                *(BattleAction *)out = *(BattleAction *)(active->action);
                Inv_BuildWeaponList(0, (WeaponListOutput *)g_MenuActiveListTarget);
            }
            if (Menu_GetBattleEquipMode()) g_MenuCommandResult = 0x197;
            else Battle_DispatchSpecialAction(0x197);
        }
        break;
    case 3:
        if (active) {
            u8 *out = g_MenuActiveListTarget;
            if (out) {
                *(BattleAttributes *)out = *active->attributes;
                Inv_BuildArmorList((BattleAttributes *)g_MenuActiveListTarget);
            }
            if (Menu_GetBattleEquipMode()) g_MenuCommandResult = 0x198;
            else Battle_DispatchSpecialAction(0x198);
        }
        break;
    case 5:
        if (active) {
            register BattleAction *data asm("$5") = active->action;
            ItemDataRecord *item = *(ItemDataRecord **)selection;
            u32 word = data->attackWord;
            int kind;
            int encoded;
            u32 flags2;
            encoded = -0x400;
            word = (word & encoded) | (item->ammo & 0x3FF);
            data->attackWord = word;
                    item = *(ItemDataRecord **)selection;
            kind = item->kind;
            data = active->action;
            if (kind != 0 && (unsigned)kind < 8) {
                encoded = kind - 4;
                if (encoded <= 0) encoded = 1;
            } else {
                            item = *(ItemDataRecord **)selection;
                kind = item->kind;
                encoded = 0;
                if ((unsigned)kind >= 19) encoded = kind - 18;
            }
            flags2 = data->attackWord;
            flags2 &= 0xFFCFFFFF;
            flags2 |= (unsigned)(encoded & 3) << 20;
            data->attackWord = flags2;
        }
        break;
    case 8: g_MenuCommandResult = 0x199; break;
    case 9: g_MenuCommandResult = -1; break;
    case 10: g_MenuCommandResult = 1000; break;
    case 11: g_MenuCommandResult = 1; break;
    case 12:
        g_InvBonusSlotCount = 4;
        Inv_SelectActiveList(0);
        BattleCmd_LoadWeaponModifiers();
        g_MenuCommandResult = 2;
        break;
    default: break;
    }
    if (g_MenuCommandResult) MenuWidget_InitPool();
}

int Menu_GetCommandResult(void) {
    return g_MenuCommandResult;
}

void Menu_ClearCommandResult(void) {
    g_MenuCommandResult = 0;
}

/* g_PlayerEntity points at the player-side field actor; its core is the
 * Combatant record used by the command paths below. */

void BattleCmd_SyncActiveAmmo(void) {
    BattleEntity *entity;
    Combatant *current;
    ItemDataRecord *entry;
    int saved;

    entity = (BattleEntity *)g_PlayerEntity;
    if (entity != 0) {
        current = entity->core;
        if (current != 0) {
            g_AyaHpCurrent[0] = current->curHP;
            if (current->action != 0) {
                saved = Inv_IsActiveListOverrideSelected();
                Inv_SelectActiveList(0);
                entry = Inv_LookupActiveListData(g_AyaEquippedWeaponSlot[0]);
                if (entry != 0) {
                    entry->ammo = current->action->attackWord & 0x3FF;
                }
                Inv_SelectActiveList(saved);
            }
        }
    }
}

void BattleCmd_SetCurrentHP(int arg0) {
    Combatant *current;

    if (g_PlayerEntity != 0) {
        current = ((BattleEntity *)g_PlayerEntity)->core;
        if (current != 0) {
            current->curHP = arg0;
        }
    }

    g_AyaHpCurrent[0] = arg0;
}

/* MASPSX_FORCE_G0 keeps the player pointer absolute, as retail; the COMMON
 * stack top supplies the small-data metadata of the one $gp-relative word. */
BattleCmdEntry *g_BattleCmdStackTop;

int BattleCmd_GetRemainingAmmo(int *out) {
    BattleCmdEntry *cursor;
    BattleEntity *entity;
    Combatant *current;
    int remaining = 0;

    entity = (BattleEntity *)g_PlayerEntity;
    if (entity != 0) {
        if (entity->core != 0) {
            current = entity->core;
            remaining = (s16)(current->exp_or_acc >> 16);
            cursor = g_BattleCmdStack;

            while (cursor < g_BattleCmdStackTop) {
                /* retail compares the whole first word, not the opcode byte */
                if (cursor->header.word == 1) {
                    remaining -= cursor->payload.ammo_spend.amount;
                }
                cursor++;
            }

            if (out != 0) {
                *out = (s16)(current->maxAtk >> 16);
            }
        }
    }

    return remaining;
}

void BattleCmd_SetCurrentMP(int arg0) {
    Combatant *current;

    if (g_PlayerEntity != 0) {
        current = ((BattleEntity *)g_PlayerEntity)->core;
        if (current != 0) {
            current->exp_or_acc = arg0 << 16;
        }
    }
}

void BattleCmd_CommitAndSyncAmmo(int item) {
    BattleEntity *entity;
    Combatant *current;
    ItemDataRecord *entry;
    int saved;

    Battle_ApplyDamage(item);
    entity = (BattleEntity *)g_PlayerEntity;
    if (entity != 0) {
        current = entity->core;
        if (current != 0) {
            g_AyaHpCurrent[0] = current->curHP;
            if (current->action != 0) {
                saved = Inv_IsActiveListOverrideSelected();
                Inv_SelectActiveList(0);
                entry = Inv_LookupActiveListData(g_AyaEquippedWeaponSlot[0]);
                if (entry != 0) {
                    entry->ammo = current->action->attackWord & 0x3FF;
                }
                Inv_SelectActiveList(saved);
            }
        }
    }
}

/* Applies a weapon change, then writes the loaded ammo back to the item. */
void BattleCmd_ChangeWeaponAndSync(int arg0) {
    Combatant *current;
    ItemDataRecord *entry;
    int saved;

    Battle_ApplySpellEffect(arg0, D_8009D254);
    if (D_8009D254 != 0) {
        current = D_8009D254->core;
        if (current != 0) {
            g_AyaHpCurrent[0] = current->curHP;
            if (current->action != 0) {
                saved = Inv_IsActiveListOverrideSelected();
                Inv_SelectActiveList(0);
                entry = Inv_LookupActiveListData(g_AyaEquippedWeaponSlot[0]);
                if (entry != 0) {
                    entry->ammo = current->action->attackWord & 0x3FF;
                }
                Inv_SelectActiveList(saved);
            }
        }
    }
}

int Aya_HasParasiteSpell(int spell)
{
    return (g_AyaParasiteSpellFlags[0] >> spell) & 1;
}

void Aya_UnlockParasiteSpell(int spell) {
    g_AyaSaveState.parasite_spell_flags |= 1 << spell;
}

int Math_IntSqrt(int value) {
    int result = 0;
    int shift = 30;

    do {
        int step = ((result << 2) + 1) << shift;

        result <<= 1;
        if (value >= step) {
            value -= step;
            result |= 1;
        }

        shift -= 2;
    } while (shift >= 0);

    return result;
}

int BattleCmd_CommitAmmoAndUpdate(void *out) {
    Combatant *current;
    ItemDataRecord *entry;
    int saved;
    int result;

    if (D_8009D254 != 0) {
        current = D_8009D254->core;
        if (current != 0) {
            g_AyaSaveState.current_hp = current->curHP;
            if (current->action != 0) {
                saved = Inv_IsActiveListOverrideSelected();
                Inv_SelectActiveList(0);
                entry = Inv_LookupActiveListData(g_InvTrackedSlots[0]);
                if (entry != 0) {
                    entry->ammo = current->action->attackWord & 0x3FF;
                }
                Inv_SelectActiveList(saved);
            }
        }
    }

    result = Inv_DrawSlotItemIcon();
    Inv_BuildWeaponList(0, out);
    return result;
}

static inline int ClampStat(ItemDataRecord *item, int which) {
    int value = item->baseStats[which] + item->bonusStats[which];
    int result = 999;
    if (value < 1000) result = value;
    return result;
}
void Inv_BuildWeaponList(int unused, WeaponListOutput *out) {
    ItemDataRecord *item;
    int i, kind, code, root, shift, trial, radicand;
    int product;
    Inv_SelectActiveList(0);
    item = Inv_LookupActiveListData(g_InventoryRuntime.tracked[0]);
    if (item->baseStats[0] + item->bonusStats[0] >= 1000) out->attack = 999;
    else out->attack = item->baseStats[0] + item->bonusStats[0];
    {
        int base2 = item->baseStats[1];
        int bonus2 = item->bonusStats[1];
        if (base2 + bonus2 < 1000) product = (base2 + bonus2) * 22500;
        else product = 0x156fabc;
        radicand = product;
    }
    root = 0;
    for (shift = 30; shift >= 0; shift -= 2) {
        trial = ((root << 2) + 1) << shift;
        root <<= 1;
        if (radicand >= trial) { radicand -= trial; root |= 1; }
    }
    out->range = root;
    out->kind = item->kind;
    out->packed.bits.count = item->ammo;
    out->packed.bits.power = ClampStat(item, 2);
    kind = item->kind;
    if (kind != 0 && (unsigned)kind < 8) {
        code = kind - 4;
        if (code <= 0) {
            code = 1;
        }
    } else {
        code = 0;
        if ((unsigned)kind >= 19) code = kind - 18;
    }
    out->effects.word = 0x11;
    out->packed.bits.category = code;
    for (i = 0; i < item->tailCount; i++) {
        int effect = item->tailData[i] & 31;
        switch (effect) {
        case 1: case 2: case 3: case 4: case 5:
            out->effects.bits.actions = g_WeaponEffectCodes[effect];
            break;
        case 6: case 7: case 8:
            out->effects.bits.mode = effect - 5;
            break;
        case 9: out->effects.bits.element8 = 1; break;
        case 10: out->effects.bits.element9 = 1; break;
        case 11: out->effects.bits.element10 = 1; break;
        case 12: out->effects.bits.element11 = 1; break;
        case 13: out->effects.bits.element12 = 1; break;
        case 14: out->effects.bits.flag15 = 1; break;
        case 15: out->effects.bits.flag16 = 1; break;
        case 16: case 17:
            out->effects.bits.hitState = effect - 15;
            break;
        case 18: out->effects.bits.flag17 = 1; break;
        case 19: case 20:
            out->effects.bits.slot = effect - 17;
            break;
        }
    }
    if (out->effects.bits.hitState == 1) out->attack = (s16)out->attack >> 1;
}

void BattleCmd_LoadWeaponModifiers(void)
{
    int saved = Inv_IsActiveListOverrideSelected();
    int i;
    ItemDataRecord *item;
    Inv_SelectActiveList(0);
    g_InvBonusSlotCount = 0;
    for (i = 6; i >= 0; --i) g_StatScaleBase[i] = 0;
    /* Historical function name notwithstanding, this is the armor index. */
    item = Inv_LookupActiveListData(g_AyaEquippedArmorSlot[0]);
    if (item) {
        for (i = 0; i < item->tailCount; ++i) {
            unsigned mod = item->tailData[i] & 31;
            /* These are decoded low-five-bit values, not full ItemMod IDs. */
            switch (mod) {
            case 8: case 9: case 10: g_InvBonusSlotCount = 1 << (mod - 8); break;
            case 11: g_StatScaleBase[0] = 3; break;
            case 12: g_StatScaleBase[1] = 2; break;
            case 13: g_StatScaleBase[5] = -2; break;
            case 15: g_StatScaleBase[1] = -2; break;
            }
        }
    }
    Inv_RecalcSlotStats();
    Inv_SelectActiveList(0);
    Inv_SelectActiveList(saved);
}

int Battle_GetScaledMaxHP(int level) {
    unsigned short maxHp;

    maxHp = Aya_LookupLevelStats(level)->hp;
    return ((g_StatScaleBase[0] + 0x14) * maxHp) / 20;
}
