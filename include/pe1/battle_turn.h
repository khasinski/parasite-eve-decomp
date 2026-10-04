#ifndef PE1_BATTLE_TURN_H
#define PE1_BATTLE_TURN_H

#include "pe1/battle.h"
#include "pe1/battle_palette.h"

/* Matching views for the turn-phase TUs. Only element zero is accessed: these
 * wide declarations preserve retail absolute references under -G8. Separate
 * read/write views also prevent GCC from retaining symbol base addresses.
 * They describe code-generation requirements, not arrays in the game data.
 */
extern BattleEntity *g_BattleTurnResumedPlayerView[16] asm("D_8009D254");
extern BattleEntity *g_BattleTurnActionPlayerView[16] asm("D_8009D254");
extern s32 g_BattleTurnPreviousFrameView[16] asm("D_8009D29C");
extern u16 g_BattleTurnResumeCountWrite[16] asm("D_8009D298");
extern s32 g_BattleTurnGameFlagsWrite[16] asm("D_8009D1A0");
extern s32 g_BattleTurnMoveLockWrite[16] asm("D_8009D2E8");
extern s32 g_BattleTurnDrawSlotView[16] asm("D_8009CDDC");
extern s32 g_BattleTurnGameFlagsRead[16] asm("D_8009D1A0");
extern BattleEntity *g_BattleTurnEntityListView[16] asm("D_8009D20C");
extern s32 g_BattleTurnFrameCounterView[16] asm("D_8009D250");
extern BattleEntity *g_BattleTurnPlayerView[16] asm("D_8009D254");
extern Combatant *D_8009D278;
extern s32 D_8009D28C;
extern u16 g_BattleTurnResumeCountRead[16] asm("D_8009D298");
extern u8 g_BattleTurnResumeModeView[16] asm("D_8009D29A");
extern s32 g_BattleTurnResumeFrameView[16] asm("D_8009D29C");
extern s32 g_BattleTurnMoveLockRead[16] asm("D_8009D2E8");


extern u8 D_8009CE74;
int Battle_ProcessActionSlot(BattleEntity *);
void Battle_ResetEnemyStats(int);
int CD_StepReadState(int);
void Battle_ClearMotionTable(void);
void Pm_StopAllBoth(void);

void Akao_Cmd_21(int, int);
int Asset_LoadTimTextures(int);
void Battle_DrawStatusPanel(int, BattleStatusPanel *);
void Battle_UpdateEnemy(BattleEntity *);
void Entity_SetActionMode(BattleEntity *, int);
void Entity_TickAnimSequences(BattleEntity *);
void Tbl_ResetAll(void);
void Battle_PhaseInitEnemyTurn(void);
void Battle_PhaseEndTurn(void);
void Battle_PhaseHitReaction(void);

#endif
