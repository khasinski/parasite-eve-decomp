#ifndef PE1_BATTLE_ENTITY_ANIM_H
#define PE1_BATTLE_ENTITY_ANIM_H

#include "pe1/asset_find.h"
#include "pe1/entity_animation.h"

extern u32 g_GameStateFlags;
extern struct FieldActor *g_FieldActorListHead;
extern BattleEntity *D_8009D254;
extern struct BattleEntity *D_8009CD10;
extern s8 g_BattleLiveEnemyCount;
extern u16 g_BattlePeBonusReward;
extern u32 g_BattleExpReward;
extern BattleRewardSlot g_BattleRewardSlots[];

void Anim_SetInterpRate(RenderObjectEntity *anim, int rate);
void Akao_Cmd_21(int arg0, int arg1);
void Battle_SlotFree(void **combatantPtr);
void Battle_StartDeathAnim(void);
void Battle_StepEntityAnimState(BattleEntity *entity);
int Entity_TriggerAnimEvent(BattleEntity *entity, u8 slot);

#endif
