/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/battle_runtime.h"

extern struct { BattleEntity *value; char pad[12]; } D_8009D20C_absolute __asm__("D_8009D20C");
extern struct { BattleEntity *value; char pad[12]; } D_8009D254_absolute __asm__("D_8009D254");
#define LIST_HEAD (D_8009D20C_absolute.value)
#define PLAYER (D_8009D254_absolute.value)

extern RenderObjectEntity D_800B0CEC;
/* These are the flags_9C and variant_visible fields of D_800B0CEC.
 * Separate views preserve the retail absolute load/store sequence under -G8. */
extern struct { u16 value; char pad[14]; } D_800B0D88_read __asm__("D_800B0D88");
extern struct { u16 value; char pad[14]; } D_800B0D88_write __asm__("D_800B0D88");
extern struct { u8 value; char pad[15]; } D_800B0D8A_absolute __asm__("D_800B0D8A");
#define GLOBAL_FLAGS_READ (D_800B0D88_read.value)
#define GLOBAL_FLAGS_WRITE (D_800B0D88_write.value)
#define GLOBAL_VISIBLE (D_800B0D8A_absolute.value)
s32 Pm_StopUpperHalf(void);
void Akao_Cmd_21(int arg0, int arg1);

void Battle_StepEscapeOrDeath(void)
{
    BattleEntity *entity;

    switch (D_8009CE74) {
    case 0:
        D_8009D244 = 0;
        Pm_StopUpperHalf();
        for (entity = LIST_HEAD; entity != 0; entity = entity->next) {
            if (entity->core == 0) {
                if (entity->parent == 0) continue;
            } else if (entity == PLAYER) {
                Anim_SetInterpRate(&D_800B0CEC, 30);
                GLOBAL_FLAGS_WRITE = GLOBAL_FLAGS_READ | 2;
                PLAYER->renderObject.flags_9C |= 2;
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
        }
        D_8009CE74++;
        break;
    case 1:
        if (PLAYER->renderObject.variant_visible != 0 ||
            GLOBAL_VISIBLE != 0) break;
        Battle_ResetEnemyStats(0);
        D_8009CE74++;
        entity = LIST_HEAD;
        if (entity != 0) {
            BattleEntity *player = PLAYER;
            do {
                if (entity == player) continue;
                if (entity->core != 0 || entity->parent != 0)
                    entity->renderObject.flags_9C |= 2;
            } while ((entity = entity->next) != 0);
        }
        break;
    case 2: {
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
            Akao_Cmd_21(0, 255);
            Pm_StopAllBoth();
            D_8009CE74++;
        }
        break;
    }
    case 3:
        if (CD_StepReadState(0) != 1) {
            Battle_ClearMotionTable();
            Entity_SetActionMode(PLAYER, 0x15);
            Battle_SetupPlayerPalette();
            D_8009D28C = 10;
        }
        break;
    }
}
