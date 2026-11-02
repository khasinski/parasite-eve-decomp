#ifndef PE1_BATTLE_START_H
#define PE1_BATTLE_START_H

#include "pe1/entity_animation.h"
#include "pe1/save_slot_metadata.h"

extern Combatant *g_ActiveActor;
extern BattleEntity *D_8009D254;
extern struct BattleEntity *D_8009CD10;
extern BattleRewardSlot g_BattleRewardSlots[10];
extern BattleEntity *g_BattleActionActors[7];
extern int g_BattleFrameCounter;
extern int g_BattleModeState;
extern u8 g_BattleSuspendedModeState, g_BattleStartClearedByteWriteOnly, g_BattleGaugeCapLatch, g_BattleStepIndex;
extern u8 g_BattleSaveOverlayActive, g_BattleSpritePrimCountdown;
extern u32 g_BattleExpReward;
extern u16 g_BattlePeBonusReward;

extern unsigned int g_BattleResultFlags;
extern unsigned int g_GameStateFlags;
extern unsigned int D_8009D2E8;
extern unsigned int D_8009CDA8;
extern int g_BattleAttackAnimFrame;
extern u32 g_FrameRngCounter;
extern s16 g_BattlePlayerAimHeight; /* battle camera height: player's model Y - 100 */

void Save_ResetGlobalFlags(void);
void srand(unsigned int seed);
void Battle_ResetEnemyStats(int mode);
void Battle_SetupEnemyAnims(void);
void Battle_CheckDropChance(void);
int Entity_CheckActionIdMatch(void);
void Battle_SetWindowBoundsByMode(int mode);
void Battle_StartEncounter(int mode);

#endif
