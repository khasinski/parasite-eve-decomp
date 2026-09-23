#ifndef PE1_BATTLE_LEVEL_UP_H
#define PE1_BATTLE_LEVEL_UP_H

#include "pe1/battle.h"

extern BattleEntity *D_8009D254[4];
extern BattleRewardSlot D_800A7FF0[10];
extern u8 D_8009CE74;
extern s16 D_8009D2A4;
extern u32 D_8009D304;
extern u16 D_8009D21C;
extern u32 D_8009D28C;

extern u32 battle_step_flags_read[4] __asm__("D_8009D1A0");
extern u32 battle_step_flags_write[4] __asm__("D_8009D1A0");
extern u32 battle_step_game_word[4] __asm__("D_800B0CD8");

void Pm_StopAllBoth(void);
void Aya_SetTotalExp(u32 exp, u16 pe_bonus, void *rewards);
void Render_BeginSceneLoad(void);
void Entity_SetActionMode(BattleEntity *entity, int mode);
int CD_StepReadState(int mode);
void Battle_SetupPlayerPalette(void);
void Battle_StepLevelUp(void);

#endif
