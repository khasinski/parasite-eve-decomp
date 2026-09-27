#ifndef PE1_BATTLE_ENTITY_ANIM_H
#define PE1_BATTLE_ENTITY_ANIM_H

#include "pe1/battle.h"

/* Animation-event view of an entity core. The six inline event records
 * overlap fields in EnemyCombatant's battle-state view; keep the views
 * separate until their shared storage/lifetime is established.
 */
typedef struct EntityAnimEventCore {
/* 0x00 */ u32 flags;
/* 0x04 */ u8 rank;
/* 0x05 */ s8 kind;
/* 0x06 */ u8 baseMode;
/* 0x07 */ u8 reserved07;
/* 0x08 */ u8 reserved08[0x10];
/* 0x18 */ EnemyActionEffect *active;
/* 0x1C */ EnemyActionEffect records[6];
/* 0x7C */ u8 reserved7c[0x36];
/* 0xB2 */ u16 assetId;
} EntityAnimEventCore;

PE1_STATIC_ASSERT(sizeof(EntityAnimEventCore) == 0xB4, entity_anim_event_core_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(EntityAnimEventCore, active) == 0x18,
                  entity_anim_event_active_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(EntityAnimEventCore, records) == 0x1C,
                  entity_anim_event_records_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(EntityAnimEventCore, assetId) == 0xB2,
                  entity_anim_event_asset_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(EnemyActionEffect, enterStep) == 4,
                  enemy_action_effect_enter_step_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(EnemyActionEffect, exitStep) == 8,
                  enemy_action_effect_exit_step_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(EnemyActionEffect, frame) == 15,
                  enemy_action_effect_frame_offset);

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
