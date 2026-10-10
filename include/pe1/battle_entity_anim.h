#ifndef PE1_BATTLE_ENTITY_ANIM_H
#define PE1_BATTLE_ENTITY_ANIM_H

#include "pe1/battle.h"

extern u32 D_8009D1A0;
extern BattleEntity *g_FieldActorListHead;
extern BattleEntity *D_8009D254;
extern Combatant *g_ActiveActor;
extern s8 D_8009D2A0;
extern u16 D_8009D21C;
extern u32 D_8009D304;
extern BattleRewardSlot D_800A7FF0[];

void Anim_SetInterpRate(RenderObjectEntity *anim, int rate);
void Akao_Cmd_21(int arg0, int arg1);
void Asset_Find08w(int id, int arg1, int x, int y, int z);
void Battle_SlotFree(void *entity);
void Battle_StartDeathAnim(void);
void Battle_StepEntityAnimState(BattleEntity *entity);
void Entity_SetActionMode(BattleEntity *entity, int mode);
int Entity_TriggerAnimEvent(BattleEntity *entity, u8 slot);

#endif
