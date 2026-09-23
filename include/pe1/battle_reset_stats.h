#ifndef PE1_BATTLE_RESET_STATS_H
#define PE1_BATTLE_RESET_STATS_H

#include "pe1/battle.h"

extern Combatant *D_8009D278;
extern int D_8009D1D0;
extern u8 D_8009D234[];
extern u8 D_8009D244;
extern s16 D_8009D298[];
extern u8 D_8009D29A[];
extern u8 D_8009D29B[];
extern u32 D_8009D29C[];

/* Independent symbol views keep the original absolute load and store. */
extern u32 reset_flags_read[4] __asm__("D_8009D2E8");
extern u32 reset_flags_write[4] __asm__("D_8009D2E8");

void Battle_FlushScriptSounds(void);
void Tbl_ResetAll(void);
void Battle_ResetEnemyStats(int mode);

#endif
