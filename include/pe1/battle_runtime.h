#ifndef PE1_BATTLE_RUNTIME_H
#define PE1_BATTLE_RUNTIME_H

#include "pe1/battle.h"

extern Combatant *D_8009D278;
extern BattleEntity *D_8009D254;
extern int D_8009D200;
extern u8 D_8009D294;

typedef struct BattleInitSlot {
    BattleEntity *actor;
    s16 field04;
    s16 field06;
} BattleInitSlot;

typedef struct BattleEnemySlot {
    s32 active;
    EnemyCombatant combatant;
} BattleEnemySlot;

extern BattleInitSlot D_800BE830[45];
extern int D_8009D2FC;
extern int D_8009D258;
extern int D_8009D208;
extern u8 D_8009CE44;
extern u8 D_8009CE40;
extern u8 D_8009D2D8;
extern u8 D_8009D1DC;
extern u8 D_8009CE38[4];
extern unsigned int D_8009D1AC;
extern u8 D_8009D1CE;
extern u8 D_8009CE60;
extern u8 D_8009D1D4;
extern u8 D_8009CE3C;
extern unsigned int D_8009D2E8;
extern int D_8009D28C;
extern BattleAction D_800A76D8;
extern s8 D_8009D2B0;
extern BattleEntity *D_8009D20C;
extern void *D_8009D1F8;
extern u8 D_8009159C[];
extern u8 D_800915C0[];
extern const EnemyCombatant D_800109B0;
extern BattleEnemySlot D_800A5D58[7];
extern u8 D_8009D2EC;
extern u8 D_8009D2A0;
extern u8 D_8009CE74;
extern u8 D_8009D244;

void Battle_FinalizeAttackResult(void);
void Battle_Init(void);
void Battle_StepScriptEntry(void);
void Pm_StopAll(void);
int Pm_SendCmd(int command, int arg1, int arg2, int arg3, int arg4, int arg5);
int Akao_SetPos3D(int arg0, int arg1, int x, int y, int z);
int Asset_Find08Alt(int id, int arg1, int x, int y, int z);
void Tbl_ResetAll(void);
void Battle_InitFadeVars(void);
void Battle_UpdatePlayerTurn(void);
void Battle_ApplyPlayerHit(void);
void Battle_AdvancePhase(void);
void BattleCmd_UndoPending(void);
void Entity_SetActionMode(BattleEntity *entity, int mode);
int Battle_RollEscapeChance(void);
void Battle_CopyPadStateToRecord(void);
void Battle_FillActionQueue(void);
void Battle_AdvanceTurnSlot(void);
void Entity_AllocSlot(BattleEntity *entity);
void Battle_StepEntityDeath(void);
void Anim_SetInterpRate(RenderObjectEntity *object, int rate);
void Pm_StopAllBoth(void);
int CD_StepReadState(int mode);
void Battle_ClearMotionTable(void);
void Battle_SetupPlayerPalette(void);
void Battle_ResetEnemyStats(int mode);
void Battle_SetupEnemyAnims(void);
void Scene_SetStoryDay(int day);

PE1_STATIC_ASSERT(sizeof(BattleInitSlot) == 8, battle_init_slot_size);
PE1_STATIC_ASSERT(sizeof(BattleEnemySlot) == 0xDC, battle_enemy_slot_size);

#endif
