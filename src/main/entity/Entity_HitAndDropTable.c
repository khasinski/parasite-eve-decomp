/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --expand-div */
#include "common.h"
#include "pe1/battle.h"

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
#define ENEMY_FIELD(base, type, member) \
    (*(type *)((char *)(base) + PE1_OFFSETOF(EnemyCombatant, member)))
#define EFFECT_FIELD(base, type, member) \
    (*(type *)((char *)(base) + PE1_OFFSETOF(EnemyActionEffect, member)))

void Entity_ResolveDropTable(void *arg0) {
    Combatant *state;
    char *entry;
    u16 scale;
    u32 flags;
    u32 word;
    int masked;
    int value;
    int roll;
    int tmp;

    state = (Combatant *)g_ActiveActor[0];
    scale = (u16)state->stat20 / 5U;
    entry = *(char **)arg0;
    {
        register u32 flags_reg asm("$6");

        flags_reg = state->stateFlags;
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
        masked = state->attributes->parameterWord.fields.first;
        if (EFFECT_FIELD(action, u8, category) == 0) {
            EFFECT_FIELD(action, u8, state) = 4;
        } else if (EFFECT_FIELD(action, u8, category) != 1) {
            EFFECT_FIELD(action, u8, state) = 3;
        }
    } else {
        int secondaryParameter = state->attributes->parameterWord.fields.second;
        masked = secondaryParameter;
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
                  (100 - (int)((Combatant *)chance_state)->attributes->parameterWord.fields.third)) /
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

#undef EFFECT_FIELD
#undef ENEMY_FIELD

extern u16 D_8009D298;
extern u8 g_BattleHitActionMode[];
extern u8 g_BattleHitActionSubmode[];
extern u32 g_BattleHitActionParam[];

int Battle_CalcRelativeAngle(BattleEntity *arg0, BattleEntity *actor);
void Entity_SetActionMode(void *actor, s32 bucket);
void Asset_Find08Alt(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);

static inline char *CurrentPlayer(void) {
    return g_PlayerEntity[0];
}

s16 Entity_ApplyHitAndSetAnim(void *arg0) {
    void *saved_arg;
    s32 value;
    char *switch_actor;
    char *actor;
    char *state;
    short bucket;
    u32 flags;

    saved_arg = arg0;
    value = 0;
    switch_actor = CurrentPlayer();

    switch (U8(switch_actor, 0xE)) {
    case 6:
    case 8:
    case 10:
    case 12:
    case 13:
    case 14:
    case 15: {
        char *case_actor;
        u8 mode;
        u8 submode;

        case_actor = CurrentPlayer();
        mode = U8(case_actor, 0xE);
        g_BattleHitActionMode[0] = mode;
        submode = U8(case_actor, 0xF);
        D_8009D298 = 1;
        g_BattleHitActionSubmode[0] = submode;
        g_BattleHitActionParam[0] = U32(case_actor, 0x14);
        break;
    }
    case 7:
    case 9:
    case 11: {
        char *case_actor;
        u8 mode;
        u8 submode;

        case_actor = CurrentPlayer();
        mode = U8(case_actor, 0xE);
        g_BattleHitActionMode[0] = mode;
        submode = U8(case_actor, 0xF);
        D_8009D298 = 1;
        g_BattleHitActionSubmode[0] = submode;
        g_BattleHitActionParam[0] = U8(case_actor, 0xF) << 16;
        break;
    }
    }

    state = g_ActiveActor[0];
    if ((U32(state, 0x4C) & 0x12000) == 0) {
        value = Battle_CalcRelativeAngle(saved_arg, CurrentPlayer());
        if ((s16)value < 0x200) {
            bucket = 0;
        } else if ((s16)value < 0x600) {
            bucket = 2;
        } else if ((s16)value < 0xA00) {
            bucket = 1;
        } else if ((s16)value < 0xE00) {
            bucket = 3;
        } else {
            bucket = 0;
        }

        Entity_SetActionMode(CurrentPlayer(), bucket);

        {
            char *effect_actor;

            effect_actor = CurrentPlayer();
            Asset_Find08Alt(0x46A, 0, S16(effect_actor, 0x2A), S16(effect_actor, 0x2E), S16(effect_actor, 0x32));
        }

        {
            char *flag_actor;

            flag_actor = CurrentPlayer();
            flags = U32(flag_actor, 0x98);
            actor = flag_actor;
        }
        if (flags & 0x100) {
            U32(actor, 0x98) = flags & ~0x100;
            D_8009D298 = 2;
        }
    }

    return (s16)value;
}
