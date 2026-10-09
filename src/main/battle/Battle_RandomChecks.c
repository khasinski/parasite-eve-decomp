#include "common.h"
#include "pe1/battle.h"

extern void *g_ActiveActor;
int rand(void);
int Battle_GetAgilityBonus(void);
void Battle_SetupEntityTarget(void *arg0);


void Battle_CheckDropChance(void)
{
    Combatant *ctx;
    int flags;
    u16 chance;

    ctx = g_ActiveActor;
    flags = ctx->action->turnWord;
    if (flags & 0x10000) {
        chance = ctx->stat22;
        if ((rand() % 100) < chance) {
            ((Combatant *)g_ActiveActor)->hpAlive = 0x2328;
        }
    }
}


void Battle_RollEnemySpawn(void *arg0)
{
    EnemyCombatant *ctx;
    int diff;
    register int threshold asm("$16");
    register int roll asm("$2");
    register int scaled asm("$3");

    ctx = ((BattleEntity *)arg0)->core;
    if (ctx->hpAlive <= 0) {
        return;
    }

    diff = (s8)(((Combatant *)g_ActiveActor)->field04.bytes.rank -
                ctx->field04.bytes.rank);
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
