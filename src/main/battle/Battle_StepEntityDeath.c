/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/battle_runtime.h"

extern struct { BattleEntity *value; char pad[12]; } D_8009D20C_absolute __asm__("D_8009D20C");
extern struct { BattleEntity *value; char pad[12]; } D_8009D254_absolute __asm__("D_8009D254");
#define LIST_HEAD (D_8009D20C_absolute.value)
#define PLAYER (D_8009D254_absolute.value)

void Battle_StepEntityDeath(void)
{
    BattleEntity *entity;

    switch (D_8009CE74) {
    case 0:
        D_8009D244 = 0;
        for (entity = LIST_HEAD; entity != 0; entity = entity->next) {
            if (entity == PLAYER) continue;
            if (entity->core == 0) {
                if (entity->parent == 0) continue;
            } else {
                u32 flags;
                Entity_SetActionMode(entity,
                    (u16)(s8)((EnemyCombatant *)entity->core)->field06.bytes.low);
                flags = entity->entityFlags;
                entity->motionX = 0;
                entity->motionY = 0;
                entity->motionZ = 0;
                entity->entityFlags = flags | 0x1000;
                asm volatile("" : : : "memory");
                if ((entity->entityFlags & 0x40000000) &&
                    !((EnemyCombatant *)entity->core)->deathPersist) {
                    entity->entityFlags |= 0x10;
                    entity->core = 0;
                }
            }
            Anim_SetInterpRate(&entity->renderObject, 30);
            entity->renderObject.flags_9C |= 2;
        }
        D_8009CE74++;
        break;
    case 1: {
        int all_done = 1;
        entity = LIST_HEAD;
        if (entity != 0) {
            BattleEntity *player = PLAYER;
            do {
                if (entity == player) continue;
                if (entity->core == 0 && entity->parent == 0) continue;
                if (entity->renderObject.variant_visible == 0) {
                    entity->core = 0;
                    entity->entityFlags |= 0x10;
                } else if (entity->entityFlags & 0x40) {
                    entity->entityFlags |= 0x10;
                    entity->core = 0;
                } else {
                    all_done = 0;
                }
            } while ((entity = entity->next) != 0);
        }
        if ((u8)all_done) {
            Pm_StopAllBoth();
            D_8009CE74++;
        }
        break;
    }
    case 2:
        if (CD_StepReadState(0) != 1) {
            Battle_ClearMotionTable();
            Entity_SetActionMode(PLAYER, 0x15);
            Battle_SetupPlayerPalette();
            D_8009D28C = 11;
            Battle_ResetEnemyStats(0);
        }
        break;
    }
}
