#include "common.h"
#include "pe1/battle.h"
/* MASPSX_FLAGS: --expand-div */

extern char *g_ActiveActor[];
extern char *g_ActiveActor_late[] asm("g_ActiveActor");
extern char *g_ActiveActor_late2[] asm("g_ActiveActor");
extern char *g_PlayerEntity[];
extern void *g_BattlePendingEnemySpawn[];

int rand(void);
s16 Entity_ApplyHitAndSetAnim(void *arg0);
void Battle_ApplyEnemyAttack(u8 *ent);

#define U8(base, off) (*(u8 *)((char *)(base) + (off)))
#define U16(base, off) (*(u16 *)((char *)(base) + (off)))
#define S16(base, off) (*(s16 *)((char *)(base) + (off)))
#define U32(base, off) (*(u32 *)((char *)(base) + (off)))
#define S32(base, off) (*(s32 *)((char *)(base) + (off)))
#define PTR(base, off) (*(char **)((char *)(base) + (off)))
#define COMBATANT_PTR(base, member) \
    (*(char **)((char *)(base) + PE1_OFFSETOF(Combatant, member)))
#define ATTRIBUTE_U32(base, member) \
    (*(u32 *)((char *)(base) + PE1_OFFSETOF(BattleAttributes, member)))
#define ENEMY_FIELD(base, type, member) \
    (*(type *)((char *)(base) + PE1_OFFSETOF(EnemyCombatant, member)))
#define EFFECT_FIELD(base, type, member) \
    (*(type *)((char *)(base) + PE1_OFFSETOF(EnemyActionEffect, member)))

void Entity_ResolveDropTable(void *arg0) {
    char *state;
    char *entry;
    u16 scale;
    u32 flags;
    u32 word;
    int masked;
    int value;
    int roll;
    int tmp;

    state = g_ActiveActor[0];
    scale = U16(state, 0x20) / 5U;
    entry = *(char **)arg0;
    {
        register u32 flags_reg asm("$6");

        flags_reg = U32(state, 0x4C);
        flags = flags_reg;
    }

    if (flags & 0x1000) {
        scale = (u16)(((u16)scale * 3) / 10);
    }
    if (flags & 0x100) {
        scale = (u16)(((u16)scale * 3) / 10);
    }

    word = ENEMY_FIELD(entry, u32, coreFlags);
    if ((int)((word >> 21) & 7) < 3) {
        char *action;

        action = ENEMY_FIELD(entry, char *, effect);
        masked = ATTRIBUTE_U32(COMBATANT_PTR(state, attributes), parameterWord) & 0x3FF;
        if (EFFECT_FIELD(action, u8, category) == 0) {
            EFFECT_FIELD(action, u8, state) = 4;
        } else if (EFFECT_FIELD(action, u8, category) != 1) {
            EFFECT_FIELD(action, u8, state) = 3;
        }
    } else {
        masked = (ATTRIBUTE_U32(COMBATANT_PTR(state, attributes), parameterWord) >> 10) & 0x3FF;
    }

    value = EFFECT_FIELD(ENEMY_FIELD(entry, char *, effect), u16, power) -
            (u16)scale - masked;
    roll = rand();

    {
        char *chance_state;
        register int roll_mod asm("$2");
        int chance;

        roll_mod = roll % 100;
        chance_state = g_ActiveActor_late[0];
        chance = ((int)ENEMY_FIELD(entry, u8, effectChance) *
                  (100 - (int)((ATTRIBUTE_U32(COMBATANT_PTR(chance_state, attributes),
                                                   parameterWord) >> 20) & 0xFF))) /
                 100;
        if (roll_mod < chance) {
            tmp = value * 3;
            value = (s32)(tmp + ((u32)tmp >> 31)) >> 1;
            U32(chance_state, 0x4C) |= 0x8000;
        }
    }

    {
        char *after_state;

        after_state = g_ActiveActor_late2[0];
        if (!(U32(after_state, 0x4C) & 0x200)) {
            if (S32(after_state, 0x34) > 0) {
                value = (s32)(value + ((u32)value >> 31)) >> 1;
            }
            if (value > 0) {
                char *action;

                action = ENEMY_FIELD(entry, char *, effect);
                if (EFFECT_FIELD(action, u8, effectType) == 0xA) {
                    value = (s32)(value + ((u32)value >> 31)) >> 1;
                }
                U16(after_state, 0xC) = U16(after_state, 0xC) - value;
            }
            if (S16(g_ActiveActor[0], 0xC) != 0) {
                Entity_ApplyHitAndSetAnim(arg0);
                if (!(U32(g_PlayerEntity[0], 0x98) & 0x100)) {
                    g_BattlePendingEnemySpawn[0] = arg0;
                }
            }
        } else if (value > 0) {
            char *action;

            action = ENEMY_FIELD(entry, char *, effect);
            if (EFFECT_FIELD(action, u8, effectType) == 0xA) {
                value = (s32)(value + ((u32)value >> 31)) >> 1;
            }
            U32(after_state, 8) -= value * ((S32(after_state, 0x28) * 4) / S16(after_state, 0x1C));
        }
    }

    if (EFFECT_FIELD(ENEMY_FIELD(entry, char *, effect), u8, effectType) != 0) {
        Battle_ApplyEnemyAttack(entry);
    }
}

#undef ATTRIBUTE_U32
#undef COMBATANT_PTR
#undef EFFECT_FIELD
#undef ENEMY_FIELD
