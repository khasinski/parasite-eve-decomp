#ifndef PE1_BATTLE_LEVEL_UP_H
#define PE1_BATTLE_LEVEL_UP_H

#include "pe1/entity_animation.h"

extern BattleEntity *D_8009D254;
extern struct BattleEntity *D_8009CD10;
extern BattleRewardSlot g_BattleRewardSlots[10];
extern u8 g_BattleStepIndex;
extern s16 g_MenuFrameCommand;
extern u32 g_BattleExpReward;
extern u16 g_BattlePeBonusReward;
extern int g_BattleModeState;

extern u32 g_GameStateFlags;

void Pm_StopAllBoth(void);
void Aya_SetTotalExp(u32 exp, u16 pe_bonus, void *rewards);
int Render_BeginSceneLoad(void);
int CdRom_StepReadState(int mode);
void Battle_SetupPlayerPalette(void);
void Battle_StepLevelUp(void);

#endif
