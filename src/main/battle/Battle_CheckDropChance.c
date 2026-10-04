#include "common.h"
#include "pe1/battle.h"

extern void *g_ActiveActor;

int rand(void);

#define COMBATANT_FIELD(base, type, member) \
    (*(type *)((char *)(base) + PE1_OFFSETOF(Combatant, member)))
#define ACTION_FIELD(base, type, member) \
    (*(type *)((char *)(base) + PE1_OFFSETOF(BattleAction, member)))

void Battle_CheckDropChance(void)
{
    void *ctx;
    int flags;
    u16 chance;

    ctx = g_ActiveActor;
    flags = ACTION_FIELD(COMBATANT_FIELD(ctx, char *, action), int, turnWord);
    if (flags & 0x10000) {
        chance = COMBATANT_FIELD(ctx, u16, stat22);
        if ((rand() % 100) < chance) {
            COMBATANT_FIELD(g_ActiveActor, short, hpAlive) = 0x2328;
        }
    }
}

#undef ENTITY_FIELD
#undef ACTION_FIELD
#undef COMBATANT_FIELD
