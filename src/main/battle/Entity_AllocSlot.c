#include "pe1/battle_runtime.h"

void Entity_AllocSlot(BattleEntity *entity) {
    EnemyCombatant initial = D_800109B0;
    u8 index;
    EnemyCombatant *allocated;

    for (index = 0; index < 7; index++) {
        if (D_800A5D58[index].active == 0) {
            D_800A5D58[index].active = 1;
            D_800A5D58[index].combatant = initial;
            allocated = &D_800A5D58[index].combatant;
            entity->core = allocated;
            D_8009D2EC++;
            allocated->field06.bytes.entityId = D_8009D2EC;
            ((EnemyCombatant *)entity->core)->field08 = 1 << index;

            if (!(entity->entityFlags & 0x2000)) {
                ((EnemyCombatant *)entity->core)->effect =
                    (EnemyActionEffect *)&((EnemyCombatant *)entity->core)->pad_1C;
                Entity_SetActionMode(entity, 2);
                D_8009D2A0++;
            }
            return;
        }
    }
}
