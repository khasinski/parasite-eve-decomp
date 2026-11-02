/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/akao.h"
#include "pe1/aya.h"
#include "pe1/battle.h"
#include "pe1/battle_modifiers.h"
#include "pe1/battle_runtime.h"
#include "pe1/game_state.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory_state.h"
#include "pe1/menu_sound.h"
#include "pe1/player_entity.h"
#include "pe1/stat_modifiers.h"
#include "pe1/textbox.h"


/* JP keeps the prompt name as two separate globals: a 6-glyph buffer and a
 * small-data count byte, instead of USA's 9-glyph TextboxNameGlyphs. */

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_InvBonusSlotCount;

int *Battle_GetModifierTable(void) {
    return g_StatScaleBase;
}

int Inv_GetBonusSlotCount(void) {
    return g_InvBonusSlotCount;
}

static inline int ClampStat(ItemDataRecord *item, int which) {
    int value = item->baseStats[which] + item->bonusStats[which];
    int result = 999;
    if (value < 1000) result = value;
    return result;
}

/* Restoring integer square root; negative inputs yield zero. */
static inline int Root(int value) {
    int root = 0, shift;
    for (shift = 30; shift >= 0; shift -= 2) {
        int trial = ((root << 2) + 1) << shift;
        root <<= 1;
        if (value >= trial) {
            value -= trial;
            root |= 1;
        }
    }
    return root;
}

void Inv_BuildArmorList(BattleAttributes *out) {
    ItemDataRecord *item;
    int value, i;
    out->effectFlags = 0;
    Inv_SelectActiveList(0);
    item = Inv_LookupActiveListData(g_InventoryRuntime.tracked[2]);
    if (item) {
        value = ClampStat(item, 0);
        out->parameterWord.fields.first = value;
        value = ClampStat(item, 1);
        out->parameterWord.fields.second = value;
        value = item->baseStats[2] + item->bonusStats[2];
        if (value >= 1000) value = 999;
        if (value < 85) value = Root(value * 3000) / 10;
        else value = (value * 249 / 208 + 402) / 10;
        out->parameterWord.fields.third = value;
        for (i = 0; i < item->tailCount; i++) {
            int effect = item->tailData[i] & 31;
            switch (effect) {
            case 1: out->effectFlags |= 1; break;
            case 2: out->effectFlags |= 2; break;
            case 3: out->effectFlags |= 4; break;
            case 4: out->effectFlags |= 8; break;
            case 5: out->effectFlags |= 16; break;
            case 6: out->effectFlags |= 0x4000; break;
            case 7: out->effectFlags |= 0x8000; break;
            case 8: case 9: case 10:
                out->effectFlags = (out->effectFlags & ~0x1E0u) |
                    (((1 << (effect - 8)) & 15) << 5);
                break;
            case 11: out->effectFlags |= 0x800; break;
            case 12: out->effectFlags |= 0x2000; break;
            case 13: out->effectFlags |= 0x10000; break;
            case 14: out->effectFlags |= 0x200; break;
            case 15: out->effectFlags |= 0x20000; break;
            case 16: out->effectFlags |= 0x400; break;
            case 17: out->effectFlags |= 0x1000; break;
            }
        }
    } else {
        out->parameterWord.fields.first = 0;
        out->parameterWord.fields.second = 0;
        out->parameterWord.fields.third = 0;
    }
    BattleCmd_LoadWeaponModifiers();
}

void Inv_RecalcSlotStats(void) {
    s16 *p;
    int level, hp;
    Combatant *actor;
    AyaLevelStats *row;

    if (D_8009CD10 && (actor = D_8009CD10->core)) {
        p = g_AyaSaveState.stats.levels;
        Aya_StatQueryLevelAndSubLevel(0, *p++, &level, 0);
        row = Aya_LookupLevelStats(level);
        hp = ((g_StatScaleBase[0] + 20) * row->hp) / 20;
        actor->maxHP = g_AyaSaveState.max_hp = hp;
        if ((s16)actor->maxHP < (s16)actor->hpMirror)
            actor->hpMirror = actor->maxHP;
        if (g_AyaSaveState.max_hp < g_AyaSaveState.current_hp)
            g_AyaSaveState.current_hp = g_AyaSaveState.max_hp;

        Aya_StatQueryLevelAndSubLevel(1, *p++, &level, 0);
        actor->stat1E = Aya_LookupLevelStats(level + g_StatScaleBase[1])->offense;
        Aya_StatQueryLevelAndSubLevel(2, *p++, &level, 0);
        actor->stat20 = Aya_LookupLevelStats(level + g_StatScaleBase[2])->defense;
        Aya_StatQueryLevelAndSubLevel(3, *p++, &level, 0);
        row = Aya_LookupLevelStats(level + g_StatScaleBase[3]);
        actor->maxAtk = row->battleMaxAtk;
        actor->atbRate = row->battleAtbRate;
        actor->atbStep = row->battleAtbStep;
        Aya_StatQueryLevelAndSubLevel(4, *p++, &level, 0);
        row = Aya_LookupLevelStats(level + g_StatScaleBase[4]);
        actor->statusStep3C = row->statusStep3C;
        actor->statusStep3E = row->statusStep3E;
        Aya_StatQueryLevelAndSubLevel(5, *p++, &level, 0);
        actor->stat22 = Aya_LookupLevelStats(level + g_StatScaleBase[5])->battleStat22;
        Aya_StatQueryLevelAndSubLevel(6, *p, &level, 0);
        actor->stat26 = Aya_LookupLevelStats(level)->inventoryCapacity;
        Inv_SetAyaSlotCount(actor->stat26);
        actor->field04.fieldId04 = g_AyaSaveState.level + 1;
    }
}

/* Reports the active-time step and rate of the current PE energy level. */
void Aya_DeriveStats(int *out_max, int *out_current) {
    int base;
    AyaLevelStats *stats;

    Aya_StatQueryLevelAndSubLevel(3, g_AyaSaveState.stats.levels[3], &base, 0);
    stats = Aya_LookupLevelStats(base + g_StatScaleBase[3]);

    if (out_max != 0) {
        *out_max = stats->battleAtbStep;
    }
    if (out_current != 0) {
        *out_current = stats->battleAtbRate;
    }
}

/* Recomputes the battle stats, then refills the player's current HP. */
void Battle_SyncEquipSlots(void) {
    Combatant *actor;
    u16 maxHP;

    Inv_RecalcSlotStats();
    if (D_8009CD10 != 0) {
        actor = D_8009CD10->core;
        if (actor != 0) {
            maxHP = actor->maxHP;
            actor->hpMirror = maxHP;
            actor->curHP = maxHP;
        }
    }
}

int Battle_GetEnemyEscapeFlag(void)
{
    Combatant *state;

    if (g_PlayerEntity != 0) {
        state = (Combatant *)g_PlayerEntity->state;
        if (state != 0) {
            return ((unsigned int)state->stateFlags >> 9) & 1;
        }
    }

    return 0;
}

int Battle_GetAgilityBonus(void)
{
    return g_AyaStatAgility[0];
}

unsigned int Aya_GetActiveTimeRate(void)
{
    return g_AyaStatActiveTimeRate[0];
}

int Battle_IsActiveWrapped(void)
{
    return (signed char)Battle_IsActive();
}

int Battle_IsInputAllowedWrapped(void)
{
    return (signed char)Pad_IsMenuConfirmAvailable();
}

int Battle_GetStateFlag1(void)
{
    return (g_GameStateFlags >> 1) & 1;
}

void Menu_CopyPromptCodes(unsigned char *src) {
    unsigned char *dst;
    unsigned char *end;
    int value;

    dst = g_TextboxNameGlyphBuf;
    end = dst + 6;
    while (dst < end) {
        value = *src;
        if (value == 0xFF) {
            break;
        }
        *dst++ = value;
        src++;
    }

    g_TextboxNameGlyphCount = dst - g_TextboxNameGlyphBuf;
}

void Menu_PlayConfirmSound(void) {
    if (g_AkaoBgmHandle[0] != 0) {
        Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x44C, 0x100, 0x80, 0x7F);
    }
}

void Menu_PlayCancelSound(void) {
    if (g_AkaoBgmHandle[0] != 0) {
        Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x44D, 0x100, 0x80, 0x7F);
    }
}

void Menu_PlayMoveSound(void) {
    if (g_AkaoBgmHandle[0] != 0) {
        Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x44E, 0x100, 0x80, 0x7F);
    }
}

void Menu_PlayErrorSound(void) {
    if (g_AkaoBgmHandle[0] != 0) {
        Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x44F, 0x100, 0x80, 0x7F);
    }
}
