#include "common.h"
#include "pe1/battle.h"

#define ENEMY_FIELD(base, type, member) \
    (*(type *)((base) + PE1_OFFSETOF(EnemyCombatant, member)))
#define EFFECT_FIELD(base, type, member) \
    (*(type *)((base) + PE1_OFFSETOF(EnemyActionEffect, member)))

void Entity_CheckActionIdMatch(char **arg0, void *arg1, char **arg2, int arg3) {
    char *entry;
    int i;
    int kind;
    int offset;
    int value;

    entry = *arg2;
    if (entry == 0) {
        return;
    }

    if (EFFECT_FIELD(ENEMY_FIELD(entry, char *, effect), u8, state) != 1) {
        return;
    }

    kind = (ENEMY_FIELD(entry, u32, coreFlags) >> 21) & 7;
    if (kind >= 3) {
        return;
    }

    if (ENEMY_FIELD(entry, s32, hpAlive) <= 0) {
        return;
    }

    i = 0;
    arg3 = (short)arg3;
    do {
        offset = (*(u32 *)entry >> 19) & 0x1C;
        value = *(s8 *)(offset + (int)entry + (i & 0xFFFF) + 0x7C);
        if (arg3 == value) {
            *(int *)(*arg0 + 0x4C) |= 0x4000;
            ENEMY_FIELD(entry, u32, coreFlags) |= 0x80000000;
            return;
        }

        if (value < 0) {
            return;
        }

        i++;
    } while ((u16)i < 4);
}

#undef EFFECT_FIELD
#undef ENEMY_FIELD
