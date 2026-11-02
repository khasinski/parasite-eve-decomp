#ifndef PE1_BATTLE_RESET_STATS_H
#define PE1_BATTLE_RESET_STATS_H

#include "pe1/battle.h"

extern Combatant *g_ActiveActor;
extern u8 g_BattleEnemyHoldTimer[];
extern u8 g_BattleEnemyEscapeFlag;
extern u16 g_BattleHitActionPending;
extern u8 g_BattleHitActionSubmode[];

extern unsigned int D_8009D2E8;
extern unsigned int D_8009CDA8;

void Battle_FlushScriptSounds(void);
void Draw_ResetTextboxes(void);
void Battle_ResetEnemyStats(int mode);

#endif
