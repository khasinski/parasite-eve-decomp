#include "pe1/battle_runtime.h"

void Entity_AllocSlot(BattleEntity *entity) {
    BattleEnemySlot *slot;
    unsigned int index;

    for (index = 0; index < 7; index++) {
        slot = &D_800A5D58[index];
        if (slot->active == 0) {
            slot->active = 1;
            slot->combatant = D_800109B0;
            entity->core = &slot->combatant;
            D_8009D2EC++;
            slot->combatant.field06.bytes.entityId = D_8009D2EC;
            slot->combatant.field08 = 1 << index;

            if (!(entity->entityFlags & 0x2000)) {
                slot->combatant.effect =
                    (EnemyActionEffect *)&slot->combatant.pad_1C;
                Entity_SetActionMode(entity, 2);
                D_8009D2A0++;
            }
            return;
        }
    }
}
