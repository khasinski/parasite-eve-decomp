/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/battle_runtime.h"
#include "pe1/psyq_nop.h"
u8 D_8009D294;
u8 D_8009D1DC;
u8 D_8009CE38[4];
u8 D_8009D2D8;
u8 D_8009CE3C;

void Battle_SetupEntityTarget(BattleEntity *actor) {
    BattleEntity *target;
    D_8009D294 = 0;
    D_8009D1DC = D_8009D278->action->turnWord & 15;
    D_8009CE38[0] = D_8009D278->action->animMode[0];
    /* Preserve retail load delays for byte stores at nonzero GP offsets. */
    {
        u8 value = D_8009D278->action->animMode[1];
        PE1_NOP_DEP("r", value);
        D_8009CE38[1] = value;
    }
    {
        u8 value = D_8009D278->action->animMode[2];
        PE1_NOP_DEP("r", value);
        D_8009CE38[2] = value;
    }
    {
        u8 value = D_8009D278->action->animMode[3];
        PE1_NOP_DEP("r", value);
        D_8009CE38[3] = value;
    }
    target = 0;
    if ((actor->entityFlags & 0x40000000) && !((EnemyCombatant *)actor->core)->deathPersist) {
        for (target = D_8009D20C; target; target = target->next) {
            EnemyCombatant *enemy;
            if (target == D_8009D254 || target == actor) continue;
            enemy = target->core;
            if (enemy && enemy->hpAlive > 0 &&
                /* Retail tests actor here, not the candidate target. */
                target->teamId == actor->teamId && !(actor->entityFlags & 0x40000000)) break;
        }
    } else if ((actor->entityFlags & 0x6000) && (s8)((EnemyCombatant *)actor->core)->field04.bytes.field05 == 3) {
        for (target = D_8009D20C; target; target = target->next) {
            EnemyCombatant *enemy;
            if (target == D_8009D254 || target == actor) continue;
            enemy = target->core;
            if (enemy && enemy->hpAlive > 0 &&
                target->parent == actor && (s8)enemy->field04.bytes.field05 == 1) break;
        }
    } else if (!(actor->entityFlags & 0x4000)) target = actor;
    if (target) {
        unsigned int mode = D_8009D278->action->turnWord & 0xC0;
        if (mode == 0xC0 || mode == 0x40) {
            BattleInitSlot *slot;
            D_8009D1DC = 0;
            slot = &D_800BE830[D_8009CE3C];
            slot->actor = target;
            slot->field04 = 2;
            slot->field06 = (s8)D_8009D2D8;
            D_8009CE3C++;
        } else {
            while (D_8009D1DC) {
                BattleInitSlot *slot;
                s16 actionIndex = (s8)D_8009D2D8;
                D_8009D1DC--;
                slot = &D_800BE830[D_8009CE3C];
                slot->actor = target;
                slot->field04 = 1;
                slot->field06 = actionIndex;
                D_8009CE3C++;
            }
        }
        Battle_DispatchEntityEffect();
        D_8009D278->stateFlags |= 0x200000;
    }
}
