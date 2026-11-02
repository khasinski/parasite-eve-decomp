/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/akao.h"
#include "pe1/battle.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_main.h"
#include "pe1/battle_runtime.h"
#include "pe1/entity_animation.h"
#include "pe1/gte_types.h"
#include "pe1/inventory.h"
#include "pe1/pm.h"

#define NULL ((void *)0)
typedef struct { s32 words[4]; } Copy16;
typedef struct { u8 bytes[0x10]; } __attribute__((packed)) PackedCopy16;
typedef struct { u8 bytes[8]; } __attribute__((packed)) PackedCopy8;
typedef struct { u8 bytes[0x10]; } LargeSymbol;
typedef union { s32 value; } FlagValue;
/* A block-move pointer seen as a word for GCC's alignment test. */
typedef union { const u8 *bytes; u32 word; } CopyAddress;

int BattleCmd_CommitAmmoAndUpdate(); /* no prototype: retail calls it with and without the action */
s16 Battle_CalcAngleToTarget(RenderObjectEntity *object, void *target);
void Battle_HaltOnPositiveX(void);
int Battle_StepCharacterAction(BattleInitSlot *slot);

/* COMMON supplies the small-data sizes of the turn queue words to MASPSX;
 * every other word stays absolute. */
u8 g_BattleActionAnimModes[4];
u8 g_BattleActionQueueCount;
u8 g_BattleActionIndex;
s32 g_BattleAilmentPmSlot;
s32 g_BattleEffectPmSlot;
s16 g_BattlePlayerAimHeight;



void Battle_AdvancePhase(void) {
    Tbl20 sp18;
    BattleActionSoundTable sp30;
    BattleEntity *temp_v1_2;
    BattleEntity *var_a2;
    int targetFound;
    s16 temp_v0;
    s32 var_a2_2;
    s32 soundX;
    s32 soundY;
    s32 soundZ;
    s32 temp_a0_3;
    s32 temp_a0_4;
    BattleEntity *facingTarget;
    s32 temp_s0;
    s32 action;
    s32 command_value;
    s32 actor_flags;
    BattleEntity *character_state;
    s32 system_flags;
    FlagValue cleanup_flags;
    FlagValue loop_limit;
    s32 temp_v0_2;
    BattleEntity *soundActor;
    s32 temp_v1_3;
    s32 temp_v1_4;
    s32 var_a0_2;
    s32 var_s0_2;
    s32 var_s0_4;
    s32 var_v0_2;
    s32 var_v1;
    EnemyActionEffect *effect;
    s8 var_a0;
    u16 temp_a0;
    u16 var_a0_3;
    u32 temp_a2;
    u32 end_slot;
    u32 var_v1_2;
    u8 temp_v0_6;
    s32 var_s0;
    u8 var_s0_3;
    s32 var_v0_3;
    BattleEntity *queuedActor;
    BattleAction *queuedAction;
    BattleInitSlot *temp_s2;
    void *temp_t0;
    EnemyCombatant *temp_v0_4;
    BattleEntity *temp_v1_6;
    EnemyCombatant *temp_v1_8;
    void *var_a0_4;
    s32 var_a1;
    int firstSlot;
    int lastSlot;
    Combatant *actor;
    Combatant *cleanupActor;
    Combatant *aya_actor;
    BattleEntity *aya_player;
    u8 *copy_dst;
    const u8 *copy_src;
    const u8 *copy_end;
    u32 misaligned;

    var_a0 = g_BattleLiveEnemyCount;
    temp_a2 = g_BattleActionIndex;
    temp_s2 = &g_BattleActionQueue[temp_a2 & 0xFF];
    if (var_a0 == 0) {
        var_s0 = temp_a2;
        if ((temp_a2 & 0xFF) < g_BattleActionQueueCount) {
            do {
                temp_a0 = (u16)g_BattleActionQueue[var_s0 & 0xFF].command;
                if (temp_a0 - 3U < 0x180U) {
                    Inv_AddItem((s16) temp_a0 - 3);
                }
                var_s0 += 1;
            } while ((var_s0 & 0xFFU) < g_BattleActionQueueCount);
        }
        g_BattleActionQueueCount = 0;
        g_BattleActionIndex = 0;
        g_GameStateFlags = g_GameStateFlags & ~0x100;
        return;
    }
    if (g_ActiveActor->stateFlags & 0x200000) {
        sp18 = g_BattleActionEffectIds;
        copy_dst = (u8 *)sp30.soundId;
        copy_src = (const u8 *)g_BattleActionSoundIds.soundId;
        copy_end = copy_src + 0x20;
        {
            CopyAddress dst;
            CopyAddress src;

            dst.bytes = copy_dst;
            src.bytes = copy_src;
            misaligned = (dst.word | src.word) & 3;
        }
        if (misaligned) {
            do {
                *(PackedCopy16 *)copy_dst = *(const PackedCopy16 *)copy_src;
                copy_src += 0x10;
                copy_dst += 0x10;
            } while (copy_src != copy_end);
            *(PackedCopy8 *)copy_dst = *(const PackedCopy8 *)copy_src;
            command_value = temp_s2->command;
        }
        else
        {
        goto aligned_copy;
block_found:
        var_a2 = temp_v1_2;
        targetFound = 1;
        goto block_search_done;
aligned_copy:
        do {
            *(Copy16 *)copy_dst = *(const Copy16 *)copy_src;
            copy_src += 0x10;
            copy_dst += 0x10;
        } while (copy_src != copy_end);
copy_tail:
        *(PackedCopy8 *)copy_dst = *(const PackedCopy8 *)copy_src;
        command_value = temp_s2->command;
        }
after_command:
        action = command_value - 0x183;
        if (temp_s2->actor != D_8009CD10) {
            temp_v0 = Battle_CalcAngleToTarget(&temp_s2->actor->renderObject, &D_8009CD10->posX);
            var_a0_2 = g_BattleActionIndex;
            firstSlot = var_a0_2 & 0xFF;

            D_8009CD10->facingAngle = temp_v0;
            targetFound = 0;
            if (g_BattleActionQueue[firstSlot].command - 0x183 == 0x13) {
                var_a2 = NULL;
                var_s0 = var_a0_2;
                lastSlot = firstSlot + 7;
                if (firstSlot < lastSlot) {
                    loop_limit.value = lastSlot;
                    do {
                        temp_v1_2 = g_BattleActionQueue[var_s0 & 0xFF].actor;
                        if ((temp_v1_2 == NULL) || (((EnemyCombatant *)temp_v1_2->core)->hpAlive <= 0) || (*(s32 *)&temp_v1_2->entityFlags & 0x4000)) {
                            var_s0 += 1;
                        } else {
                            goto block_found;
                        }
                    } while ((var_s0 & 0xFF) < loop_limit.value);
                }
block_search_done:
                if (!(targetFound & 0xFF)) {
                    var_v0_3 = g_BattleActionIndex + 7;
                temp_v1_3 = var_v0_3 & 0xFF;
                temp_v1_3 <<= 3;
                {
                    BattleInitSlotAddress at;

                    at.slot = g_BattleActionQueue;
                    at.address = temp_v1_3 + at.address;
                    temp_v1_3 = (u16)at.slot->command;
                }
                g_BattleActionIndex = var_v0_3;
                temp_v1_3 -= 3;
                temp_v1_3 = temp_v1_3 < 0x194U;
                if (!temp_v1_3 || (D_8009CD10->actionMode < 4U)) {
                    g_GameStateFlags = g_GameStateFlags & ~0x100;
                }
                Entity_SetActionMode(((void *)D_8009CD10), g_ActiveActor->actionMode12);
                g_ActiveActor->stateFlags |= 0x200000;
                return;
                }
                if (var_a2 != NULL) {
                    var_s0 = g_BattleActionIndex;
                    temp_v1_3 = var_s0 & 0xFF;
                    var_a0_2 = temp_v1_3 + 7;
                    if (temp_v1_3 < var_a0_2) {
                        var_a1 = var_a0_2;
                        do {
                            queuedActor = g_BattleActionQueue[temp_v1_3].actor;
                            if ((((EnemyCombatant *)queuedActor->core)->hpAlive <= 0) || (queuedActor == NULL)) {
                                g_BattleActionQueue[temp_v1_3].actor = var_a2;
                            }
                            var_s0 += 1;
                            temp_v1_3 = var_s0 & 0xFF;
                        } while (temp_v1_3 < var_a1);
                    }
                }
            }
            else
            {
            temp_v1_6 = g_BattleActionQueue[firstSlot].actor;
            if ((temp_v1_6 == NULL) || (((EnemyCombatant *)temp_v1_6->core)->hpAlive <= 0) || (*(s32 *)&temp_v1_6->entityFlags & 0x4000)) {
                var_v0_3 = g_BattleActionIndex + 1;
block_35:
                temp_v1_3 = var_v0_3 & 0xFF;
                temp_v1_3 <<= 3;
                {
                    BattleInitSlotAddress at;

                    at.slot = g_BattleActionQueue;
                    at.address = temp_v1_3 + at.address;
                    temp_v1_3 = (u16)at.slot->command;
                }
                g_BattleActionIndex = var_v0_3;
                temp_v1_3 -= 3;
                temp_v1_3 = temp_v1_3 < 0x194U;
                if (!temp_v1_3 || (D_8009CD10->actionMode < 4U)) {
                    g_GameStateFlags = g_GameStateFlags & ~0x100;
                }
                Entity_SetActionMode(((void *)D_8009CD10), g_ActiveActor->actionMode12);
                g_ActiveActor->stateFlags |= 0x200000;
                return;
            }
        }
            }
block_39:
        Pm_StopAll();
        if (action == 6) {
            BattleEntity *attackTarget;
            queuedAction = g_ActiveActor->action;
            if (!(*(s32 *)&queuedAction->attackWord & 0x3FF)) {
                BattleCmd_CommitAmmoAndUpdate(queuedAction);
            }
            g_ActiveActor->stateFlags |= 0x100000;
            attackTarget = temp_s2->actor;
            temp_s0 = attackTarget->renderObject.target_y - g_BattlePlayerAimHeight;
            temp_v0_2 = ratan2(temp_s0, Battle_CalcDistToPlayer(attackTarget, ((void *)D_8009CD10)));
            if (temp_v0_2 < -0xAB) {
                var_v1 = 0;
            } else {
                var_v1 = 2;
                if (temp_v0_2 < 0xE4) {
                    var_v1 = 1;
                }
            }
            Entity_SetActionMode(((void *)D_8009CD10), *(var_v1 + g_ActiveActor->targetAngleActionModes));
            {
                s32 *stateFlags = &g_ActiveActor->stateFlags;
                *stateFlags &= 0xFFDFFFFF;
            }
            g_BattleActionAnimModes[1] = 0x58;
            g_BattleActionAnimModes[0] = 0;
            g_BattleActionAnimModes[2] = 0x18;
            g_BattleActionAnimModes[3] = 0;
        } else {
            Entity_SetActionMode(((void *)D_8009CD10), 0xFU);
        }
        if (action != 0x13) {
            g_BattleEffectPmSlot = Scene_LoadRoomAssets(sp18.bytes[action], temp_s2->actor);
        }
        if ((action - 7U < 2U) || (action == 0xA)) {
            soundActor = temp_s2->actor;
            var_a0_3 = sp30.soundId[action];
            soundX = soundActor->renderObject.target_x;
            soundY = soundActor->renderObject.target_y;
            soundZ = soundActor->renderObject.target_z;
        } else {
            var_a0_3 = sp30.soundId[action];
            soundX = D_8009CD10->posX.parts.integer;
            soundY = D_8009CD10->posY.parts.integer;
            soundZ = D_8009CD10->posZ.parts.integer;
        }
        Akao_SendPositionalCmdStereo(var_a0_3, 0, soundX, soundY, soundZ);
        var_a1 = 0xFFDFFFFF;
        var_a0_4 = g_ActiveActor;
        actor_flags = ((Combatant *)var_a0_4)->stateFlags;
        g_GameStateFlags = g_GameStateFlags | 0x100;
        actor_flags &= var_a1;
        ((Combatant *)var_a0_4)->stateFlags = actor_flags;
    }
block_55:
    actor = g_ActiveActor;
    actor_flags = actor->stateFlags;
    if (actor_flags & 0x80000) {
        if ((Battle_StepAyaAction() << 0x18) != 0) {
            end_slot = g_BattleActionIndex;
            aya_actor = g_ActiveActor;
            aya_player = D_8009CD10;
            aya_actor->exp_or_acc = 0;
            aya_player->entityFlags &= ~0x100;
            var_s0_4 = end_slot - 7;
            var_a2_2 = var_s0_4 & 0xFF;
            if (var_a2_2 < end_slot) {
                do {
                    facingTarget = g_BattleActionQueue[var_a2_2].actor;
                    if (facingTarget != ((void *)D_8009CD10)) {
                        Battle_UpdateEntityFacing(facingTarget);
                    }
                    var_s0_4 += 1;
                    var_a2_2 = var_s0_4 & 0xFF;
                } while ((unsigned)var_a2_2 < g_BattleActionIndex);
            }
            var_a0_4 = g_FieldActorListHead;
            if (var_a0_4 != NULL) {
                do {
                    if (var_a0_4 != D_8009CD10) {
                        temp_v0_4 = ((BattleEntity *)var_a0_4)->core;
                        if (temp_v0_4 != NULL) {
                            effect = temp_v0_4->effect;
                            if (effect != NULL) {
                                effect->state = 4;
                                temp_v1_8 = ((BattleEntity *)var_a0_4)->core;
                                temp_v1_8->coreFlags &= 0xC0FFFFFF;
                            }
                        }
                    }
                    var_a0_4 = ((BattleEntity *)var_a0_4)->next;
                } while (var_a0_4 != NULL);
            }
            g_ActiveActor->stateFlags &= 0xFFF7FFFF;
            Battle_HaltOnPositiveX();
        }
    } else if (actor_flags & 0x100000) {
        temp_v0 = Battle_CalcAngleToTarget(&temp_s2->actor->renderObject, ((void *)D_8009CD10) + 0x28);
        D_8009CD10->facingAngle = temp_v0;
        Battle_StepCharacterAction(temp_s2);
        if (g_PmSlotTable[g_BattleEffectPmSlot].header.state - 1U >= 2U) {
            var_a2_2 = 0xFFEFFFFF;
            temp_a0_3 = -0x101;
            cleanupActor = g_ActiveActor;
            character_state = D_8009CD10;
            cleanupActor->exp_or_acc = 0;
            *(s32 *)&character_state->entityFlags &= temp_a0_3;
            system_flags = g_GameStateFlags;
            cleanup_flags.value = cleanupActor->stateFlags;
            system_flags &= temp_a0_3;
            cleanup_flags.value &= var_a2_2;
            g_GameStateFlags = system_flags;
            cleanup_flags.value |= 0x200000;
            cleanupActor->stateFlags = cleanup_flags.value;
            Battle_HaltOnPositiveX();
        }
    } else {
        if (D_8009CD10->animLastFrame == D_8009CD10->animPrev.parts.integer) {
            Entity_SetActionMode(((void *)D_8009CD10), actor->actionMode12);
        }
        if (((g_PmSlotTable[g_BattleEffectPmSlot].header.state - 1U >= 2U) || (temp_s2->command == 0x196)) && (g_ActiveActor->stateFlags |= 0x200000, Battle_ApplySpellEffect(temp_s2->command - 0x183, temp_s2->actor), (temp_s2->command != 0x196))) {
            temp_v0_6 = g_BattleActionIndex + 1;
            g_BattleActionIndex = temp_v0_6;
            if (((u16)g_BattleActionQueue[temp_v0_6 & 0xFF].command - 3U >= 0x194U) || (D_8009CD10->actionMode < 4U)) {
                g_GameStateFlags = g_GameStateFlags & ~0x100;
            }
            temp_a0_4 = g_BattleActionQueueCount;
            for (var_s0_4 = g_BattleActionIndex; (var_s0_4 & 0xFFU) < (unsigned)temp_a0_4; var_s0_4++) {
                if ((u16)g_BattleActionQueue[var_s0_4 & 0xFF].command - 1U < 2U) {
                    Battle_DispatchEntityEffect();
                    return;
                }
            }
        }
    }
}

void Battle_StartEnemyAttackEffect(BattleEntity *entity)
{
    Combatant *core = entity->core;
    u32 *status = &core->statusFlags2;
    int effect;

    if ((g_ActiveActor->action->turnWord & 0x400) && ((*status & 3) != 1)) {
        effect = Scene_LoadRoomAssets(7, entity);
        g_BattleAilmentPmSlot = effect;
        Pm_SendCmd(effect, 0, 0, 2, 0, 0);
        Asset_Find08Alt(0x484, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((g_ActiveActor->action->turnWord & 0x300) == 0x300 &&
        ((*status & 0x3C000) != 0x14000)) {
        g_BattleAilmentPmSlot = Scene_LoadRoomAssets(90, entity);
        Asset_Find08Alt(0x488, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((g_ActiveActor->action->turnWord & 0x100) &&
        ((*status & 0xC000) != 0x4000)) {
        g_BattleAilmentPmSlot = Scene_LoadRoomAssets(88, entity);
        Asset_Find08Alt(0x488, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((g_ActiveActor->action->turnWord & 0x200) &&
        ((*status & 0x30000) != 0x10000)) {
        g_BattleAilmentPmSlot = Scene_LoadRoomAssets(89, entity);
        Asset_Find08Alt(0x486, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((g_ActiveActor->action->turnWord & 0x800) &&
        ((*status & 0xC) != 4)) {
        effect = Scene_LoadRoomAssets(7, entity);
        g_BattleAilmentPmSlot = effect;
        Pm_SendCmd(effect, 0, 0, 1, 0, 0);
        Asset_Find08Alt(0x482, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((g_ActiveActor->action->turnWord & 0x1000) &&
        ((*status & 0x30) != 0x10)) {
        effect = Scene_LoadRoomAssets(7, entity);
        g_BattleAilmentPmSlot = effect;
        Pm_SendCmd(effect, 0, 0, 0, 0, 0);
        Asset_Find08Alt(0x480, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
    }
}
