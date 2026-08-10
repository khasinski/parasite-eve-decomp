#include "common.h"
#include "pe1/battle.h"
extern void *g_ActiveActor;

int rand(void);
int Battle_GetAgilityBonus(void);
void Battle_SetupEntityTarget(void *arg0);

#define COMBATANT_FIELD(base, type, member) \
    (*(type *)((char *)(base) + PE1_OFFSETOF(Combatant, member)))
#define ENEMY_FIELD(base, type, member) \
    (*(type *)((char *)(base) + PE1_OFFSETOF(EnemyCombatant, member)))
#define ENTITY_FIELD(base, type, member) \
    (*(type *)((char *)(base) + PE1_OFFSETOF(BattleEntity, member)))

void Battle_RollEnemySpawn(void *arg0)
{
    void *ctx;
    int diff;
    register int threshold asm("$16");
    register int roll asm("$2");
    register int scaled asm("$3");

    ctx = ENTITY_FIELD(arg0, void *, core);
    if (ENEMY_FIELD(ctx, int, hpAlive) <= 0) {
        return;
    }

    diff = (s8)(COMBATANT_FIELD(g_ActiveActor, u8, field04.bytes.rank) -
                ENEMY_FIELD(ctx, u8, field04.bytes.rank));
    if (diff > 0) {
        threshold = Battle_GetAgilityBonus();
        roll = rand() % 100;
        scaled = roll * 10;
    } else if (diff == 0) {
        threshold = Battle_GetAgilityBonus() * 3;
        roll = rand() % 100;
        scaled = roll * 50;
    } else if (diff == -1) {
        threshold = Battle_GetAgilityBonus() * 3;
        roll = rand() % 100;
        scaled = roll * 100;
    } else if (diff < -1) {
        threshold = Battle_GetAgilityBonus();
        roll = rand() % 100;
        scaled = roll * 50;
    } else {
        return;
    }

    if (scaled < threshold) {
        Battle_SetupEntityTarget(arg0);
    }
}

#undef ENTITY_FIELD
#undef ENEMY_FIELD
#undef COMBATANT_FIELD
