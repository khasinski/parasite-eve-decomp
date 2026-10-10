#ifndef PE1_ENTITY_ANIMATION_H
#define PE1_ENTITY_ANIMATION_H

#include "pe1/battle.h"

/* Each model selects a 0xC0-byte row of action-record pointers. */
typedef struct EntityActionTable {
    void *actions[48];
} EntityActionTable;

PE1_STATIC_ASSERT(sizeof(EntityActionTable) == 0xC0,
                  entity_action_table_size);
extern EntityActionTable D_800B0E98[];

void Entity_AdvanceAnim(BattleEntity *entity);
void Entity_SetAction(BattleEntity *entity, int mode);
void Entity_SetActionMode(BattleEntity *entity, int mode);

#endif
