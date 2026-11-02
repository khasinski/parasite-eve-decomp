/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/battle.h"
#include "pe1/battle_main.h"
#include "pe1/battle_runtime.h"
#include "pe1/gte_types.h"

/* COMMON supplies the small-data sizes of the turn queue words to MASPSX;
 * every other word stays absolute. g_BattleActionAnimMode1/3 are one-byte
 * views of g_BattleActionAnimModes[1] and [3]: Battle_UpdatePlayerTurn
 * compiles to retail only with the scalar symbols. */
u8 g_BattleActionAnimMode1;
u8 g_BattleActionAnimMode3;
u8 g_BattleActionQueueCount;
u8 g_BattleActionIndex;
int g_BattleAttackPmSlot;
s16 g_BattlePlayerAimHeight;

int Battle_StepCharacterAction(BattleInitSlot *slot);

#define CORE_HEALTH(entity) (((EnemyCombatant *)(entity)->core)->hpAlive)

void Battle_UpdatePlayerTurn(void)
{

    BattleInitSlot *slot = &g_BattleActionQueue[g_BattleActionIndex];
    BattleEntity *player = D_8009CD10;
    BattleEntity *actor;
    Combatant *active;
    int angle;
    int category;
    u8 step;
    int diff;

    if (player->actionMode != 12) {
        active = g_ActiveActor;
        if (active->stateFlags & 0x200000) {
            BattleEntity *target = slot->actor;
            if (target->core != 0 && CORE_HEALTH(target) > 0) {
                diff = target->renderObject.target_y;
                diff -= g_BattlePlayerAimHeight;
                angle = ratan2(diff, Battle_CalcDistToPlayer(target, player));
                if (angle < -0xAB) category = 0;
                else if (angle < 0xE4) category = 1;
                else category = 2;
                Entity_SetActionMode(D_8009CD10, *(category + g_ActiveActor->targetAngleActionModes));
                g_ActiveActor->stateFlags &= ~0x200000;
            }
        }

        {
            BattleEntity *angle_actor = slot->actor;
            D_8009CD10->facingAngle = Battle_CalcAngleToTarget(&angle_actor->renderObject, &D_8009CD10->posX);
            step = Battle_StepCharacterAction(slot);
            if (step == 0) return;
            if (step != 1) return;
        }

        {
            if (slot->actor != g_BattleActionQueue[g_BattleActionIndex + 1].actor ||
                CORE_HEALTH(slot->actor) <= 0) {
                u8 count = g_BattleActionAnimMode1;
                u8 delta = g_BattleActionAnimMode3;
                BattleAction *effectAction = g_ActiveActor->action;
                g_BattleActionAnimMode1 = count + delta;
                asm volatile("" : : );
                if (effectAction->attackWord & 0x3FF)
                    Battle_StartEnemyAttackEffect(slot->actor);
            }
        }

        g_BattleActionIndex++;
        if (g_BattleActionIndex != g_BattleActionQueueCount) return;
        if (g_ActiveActor->action->actionCode.actionId != 8)
            Pm_SendCmd(g_BattleAttackPmSlot, 0, 0, 2, 0, 0);
        {
            int sound = g_BattleImpactPmSlot;
            if (sound != -1)
                Pm_SendCmd(sound, 0, 0, 2, 0, 0);
        }
        return;
    }

    if (player->animLastFrame != player->animPrev.parts.integer) return;
    actor = slot->actor;
    diff = actor->renderObject.target_y;
    diff -= g_BattlePlayerAimHeight;
    angle = ratan2(diff, Battle_CalcDistToPlayer(actor, player));
    if (angle < -0xAB) category = 0;
    else if (angle < 0xE4) category = 1;
    else category = 2;
    Entity_SetActionMode(D_8009CD10, *(category + g_ActiveActor->targetAngleActionModes));
}

#define U8_AT(ptr, offset) (*(u8 *)((u8 *)(ptr) + (offset)))
#define S16_AT(ptr, offset) (*(s16 *)((u8 *)(ptr) + (offset)))
#define U16_AT(ptr, offset) (*(u16 *)((u8 *)(ptr) + (offset)))
#define U32_AT(ptr, offset) (*(u32 *)((u8 *)(ptr) + (offset)))
#define ACTION_AT(index) ((u16)g_BattleActionQueue[(u8)(index)].command)

void Battle_ApplyPlayerHit(void) {
    u32 committed;
    int mask;
    unsigned int index;
    Combatant *actor;
    int flags;
    u16 action;

    committed = 0x200000;
    if (g_ActiveActor->stateFlags & committed) {
        unsigned int stateFlags;
        Entity_SetActionMode(D_8009CD10, 0xE);
        Asset_Find08Alt(0x4B3, 0, D_8009CD10->posX.parts.integer,
                        D_8009CD10->posY.parts.integer, D_8009CD10->posZ.parts.integer);

        mask = ~0x200000;
        actor = g_ActiveActor;
        stateFlags = actor->stateFlags;
        flags = g_GameStateFlags;
        g_GameStateFlags = flags | 0x100;
        actor->stateFlags = stateFlags & mask;
    }

    if (D_8009CD10->animLastFrame == D_8009CD10->animPrev.parts.integer) {
        Entity_SetActionMode(D_8009CD10, g_ActiveActor->actionMode12);
        index = g_BattleActionIndex;
        g_ActiveActor->stateFlags |= committed;

        Battle_ApplyDamage((s16)ACTION_AT(index) - 3);
        Scene_LoadRoomAssets(0x55, D_8009CD10);

        {
            unsigned int nextIndex = g_BattleActionIndex + 1;
            action = ACTION_AT(nextIndex);
            g_BattleActionIndex = nextIndex;
        }

        if (action - 3U >= 0x194U || D_8009CD10->actionMode < 4) {
            g_GameStateFlags = g_GameStateFlags & ~0x100;
        }
    }
}
