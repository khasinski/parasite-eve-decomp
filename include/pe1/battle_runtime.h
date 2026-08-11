#ifndef PE1_BATTLE_RUNTIME_H
#define PE1_BATTLE_RUNTIME_H

#include "pe1/battle.h"

extern Combatant *D_8009D278;
extern BattleEntity *D_8009D254;
extern int D_8009D200;
extern u8 D_8009D294;

typedef struct BattleInitSlot {
    s32 value;
    s16 field04;
    s16 field06;
} BattleInitSlot;

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
void Entity_SetActionMode(BattleEntity *entity, int mode);
int Battle_RollEscapeChance(void);

PE1_STATIC_ASSERT(sizeof(BattleInitSlot) == 8, battle_init_slot_size);

#endif
