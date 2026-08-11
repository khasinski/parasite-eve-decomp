#include "pe1/battle_runtime.h"

void Battle_StepEntityDeath(void) {
    BattleEntity *entity;
    EnemyCombatant *enemy;
    int allFinished;

    switch (D_8009CE74) {
    case 0:
        D_8009D244 = 0;
        entity = D_8009D20C;
        while (entity != 0) {
            if (entity != D_8009D254) {
                enemy = entity->core;
                if (enemy != 0) {
                    Entity_SetActionMode(entity, (s8)enemy->field06.bytes.low);
                    entity->motionX = 0;
                    entity->motionY = 0;
                    entity->motionZ = 0;
                    entity->entityFlags |= 0x1000;
                    if ((entity->entityFlags & 0x40000000) &&
                        enemy->deathPersist == 0) {
                        entity->entityFlags |= 0x10;
                        entity->core = 0;
                    }
                    Anim_SetInterpRate(&entity->renderObject, 30);
                    entity->renderObject.flags_9C |= 2;
                } else if (entity->parent != 0) {
                    Anim_SetInterpRate(&entity->renderObject, 30);
                    entity->renderObject.flags_9C |= 2;
                }
            }
            entity = entity->next;
        }
        D_8009CE74++;
        break;

    case 1:
        allFinished = 1;
        entity = D_8009D20C;
        while (entity != 0) {
            if (entity != D_8009D254 &&
                (entity->core != 0 || entity->parent != 0)) {
                if (entity->renderObject.variant_visible == 0) {
                    entity->core = 0;
                    entity->entityFlags |= 0x10;
                } else if (entity->entityFlags & 0x40) {
                    entity->entityFlags |= 0x10;
                    entity->core = 0;
                } else {
                    allFinished = 0;
                }
            }
            entity = entity->next;
        }
        if (allFinished) {
            Pm_StopAllBoth();
            D_8009CE74++;
        }
        break;

    case 2:
        if (CD_StepReadState(0) != 1) {
            Battle_ClearMotionTable();
            Entity_SetActionMode(D_8009D254, 0x15);
            Battle_SetupPlayerPalette();
            D_8009D28C = 11;
            Battle_ResetEnemyStats(0);
        }
        break;
    }
}
