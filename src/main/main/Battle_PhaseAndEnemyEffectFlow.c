/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: --expand-div -G8 */

#include "common.h"
#include "pe1/battle.h"

#define NULL ((void *)0)
typedef struct { u8 bytes[0x14]; } __attribute__((packed)) Tbl20;
typedef struct { s32 words[4]; } Copy16;
typedef struct { u8 bytes[0x10]; } __attribute__((packed)) PackedCopy16;
typedef struct { u8 bytes[8]; } __attribute__((packed)) PackedCopy8;
typedef struct { u8 bytes[0x10]; } LargeSymbol;
typedef struct { u16 value; u8 pad[6]; } QueueCommand;
typedef union { s32 value; } FlagValue;

void Akao_SendPositionalCmdStereo();
void BattleCmd_CommitAmmoAndUpdate();
s16 Battle_CalcAngleToTarget();
s32 Battle_CalcDistToPlayer();
void Battle_DispatchEntityEffect();
void Battle_HaltOnPositiveX();
s32 Battle_StepAyaAction();
void Battle_StepCharacterAction();
void Battle_UpdateEntityFacing();
void Entity_SetActionMode();
s32 ratan2();
void Inv_AddItem();
void Pm_StopAll();
s32 Scene_LoadRoomAssets();
extern volatile struct { Combatant *value; char pad[12]; } D_8009D278_absolute __asm__("D_8009D278");
#define ACTIVE (D_8009D278_absolute.value)
extern const Tbl20 D_8001074C;
extern const BattleActionSoundTable D_80010760;
extern LargeSymbol D_800942E4_o __asm__("D_800942E4");
extern s8 D_8009CE38;
extern s8 D_8009CE39;
extern s8 D_8009CE3A;
extern s8 D_8009CE3B;
extern u8 D_8009CE3C;
extern BattleGameStateWindow D_8009D1A0_r4 __asm__("D_8009D1A0");
extern BattleGameStateWindow D_8009D1A0_w4 __asm__("D_8009D1A0");
extern u8 D_8009D1D4;
extern LargeSymbol D_8009D20C_o __asm__("D_8009D20C");
extern void *D_8009D254_10 __asm__("D_8009D254");
extern LargeSymbol D_8009D254_13 __asm__("D_8009D254");
extern LargeSymbol D_8009D254_14 __asm__("D_8009D254");
extern LargeSymbol D_8009D254_15 __asm__("D_8009D254");
extern s32 D_8009D258;
extern LargeSymbol D_8009D278_10 __asm__("D_8009D278");
extern LargeSymbol D_8009D278_13 __asm__("D_8009D278");
extern s16 D_8009D27C;
extern LargeSymbol D_8009D2A0_o __asm__("D_8009D2A0");
#define D_800BE830_BYTES ((u8 *)&D_800BE830[0])
extern QueueCommand D_800BE834[];

#define D_800942E4 (*(u8 **)&D_800942E4_o)
#define D1A0_R0 (D_8009D1A0_r4.flags)
#define D1A0_W0 (D_8009D1A0_w4.flags)
#define D1A0_R1 (D_8009D1A0_r4.flags)
#define D1A0_W1 (D_8009D1A0_w4.flags)
#define D1A0_R2 (D_8009D1A0_r4.flags)
#define D1A0_W2 (D_8009D1A0_w4.flags)
#define D1A0_R3 (D_8009D1A0_r4.flags)
#define D1A0_W3 (D_8009D1A0_w4.flags)
#define D1A0_R4 (D_8009D1A0_r4.flags)
#define D1A0_W4 (D_8009D1A0_w4.flags)
#define D_8009D20C (*(void **)&D_8009D20C_o)
#define D254(n) (*(void **)&D_8009D254_##n)
#define D278(n) (*(void **)&D_8009D278_##n)
#define D_8009D2A0 (*(s8 *)&D_8009D2A0_o)

void Battle_AdvancePhase(void) {
    Tbl20 sp18;
    BattleActionSoundTable sp30;
    BattleEntity *temp_v1_2;
    BattleEntity *var_a2;
    int targetFound;
    s16 temp_v0;
    s32 var_a2_2;
    s32 var_a3_2;
    s32 var_v0_5;
    s32 temp_a0_3;
    s32 temp_a0_4;
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
    s32 var_v0_6;
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
    void **temp_v1_5;
    BattleAction *queuedAction;
    BattleInitSlot *temp_s2;
    void *temp_t0;
    EnemyCombatant *temp_v0_4;
    void *temp_v1;
    BattleEntity *temp_v1_6;
    EnemyCombatant *temp_v1_8;
    void *var_a0_4;
    void *var_a1;
    Combatant *actor;
    Combatant *aya_actor;
    BattleEntity *aya_player;
    u8 *copy_dst;
    const u8 *copy_src;
    const u8 *copy_end;

    var_a0 = D_8009D2A0;
    temp_a2 = D_8009D1D4;
    temp_s2 = &D_800BE830[temp_a2 & 0xFF];
    if (var_a0 == 0) {
        var_s0 = temp_a2;
        if ((temp_a2 & 0xFF) < (u8) D_8009CE3C) {
            do {
                temp_a0 = D_800BE834[var_s0 & 0xFF].value;
                if ((u32) (temp_a0 - 3) < 0x180U) {
                    Inv_AddItem((s16) temp_a0 - 3);
                }
                var_s0 += 1;
            } while ((u32) (var_s0 & 0xFF) < (u8) D_8009CE3C);
        }
        D_8009CE3C = 0;
        D_8009D1D4 = 0;
        D1A0_W0 = D1A0_R0 & ~0x100;
        return;
    }
    if (((Combatant *)D278(13))->stateFlags & 0x200000) {
        sp18 = D_8001074C;
        copy_dst = (u8 *)sp30.soundId;
        copy_src = (const u8 *)D_80010760.soundId;
        copy_end = copy_src + 0x20;
        if (((u32)copy_dst | (u32)copy_src) & 3) {
            do {
                *(PackedCopy16 *)copy_dst = *(const PackedCopy16 *)copy_src;
                copy_src += 0x10;
                copy_dst += 0x10;
            } while (copy_src != copy_end);
            *(PackedCopy8 *)copy_dst = *(const PackedCopy8 *)copy_src;
            command_value = temp_s2->field04;
            goto after_command;
        }
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
        command_value = temp_s2->field04;
after_command:
        action = command_value - 0x183;
        var_a0_2 = (s32)temp_s2->actor;
        var_a1 = D254(14);
        if (var_a0_2 != var_a1) {
            temp_v0 = Battle_CalcAngleToTarget(var_a0_2 + 0x1B4, var_a1 + 0x28);
            var_a0_2 = (s32) D_8009D1D4;
            var_a1 = (void *) (var_a0_2 & 0xFF);
            var_a2 = (BattleEntity *) ((s32) var_a1 * 8);
            ((BattleEntity *)D254(15))->facingAngle = temp_v0;
            targetFound = 0;
            if (*(s16 *)((u8 *)D_800BE834 + (s32)var_a2) - 0x183 == 0x13) {
                var_a2 = NULL;
                var_s0 = var_a0_2;
                temp_v1 = var_a1 + 7;
                if ((s32) var_a1 < (s32) temp_v1) {
                    loop_limit.value = (s32) temp_v1;
loop_17:
                    temp_v1_2 = D_800BE830[var_s0 & 0xFF].actor;
                    if ((temp_v1_2 == NULL) || (((EnemyCombatant *)temp_v1_2->core)->hpAlive <= 0) || (*(s32 *)&temp_v1_2->entityFlags & 0x4000)) {
                        var_s0 += 1;
                        if ((var_s0 & 0xFF) < loop_limit.value) {
                            goto loop_17;
                        }
                    } else {
                        goto block_found;
                    }
                }
block_search_done:
                if (!(targetFound & 0xFF)) {
                    var_v0_3 = D_8009D1D4 + 7;
                    goto block_35;
                }
                if (var_a2 != NULL) {
                    var_s0 = D_8009D1D4;
                    temp_v1_3 = var_s0 & 0xFF;
                    var_a0_2 = temp_v1_3 + 7;
                    if (temp_v1_3 < var_a0_2) {
                        var_a1 = (void *) var_a0_2;
                        do {
                            var_a0_2 = temp_v1_3 * 8;
                            temp_v1_5 = *(void ***)(D_800BE830_BYTES + var_a0_2);
                            if ((((EnemyCombatant *)*temp_v1_5)->hpAlive <= 0) || (temp_v1_5 == NULL)) {
                                *(void **)(D_800BE830_BYTES + var_a0_2) = var_a2;
                            }
                            var_s0 += 1;
                            temp_v1_3 = var_s0 & 0xFF;
                        } while (temp_v1_3 < (s32) var_a1);
                    }
                }
                goto block_39;
            }
            temp_v1_6 = *(void **)(D_800BE830_BYTES + (s32)var_a2);
            if ((temp_v1_6 == NULL) || (((EnemyCombatant *)temp_v1_6->core)->hpAlive <= 0) || (*(s32 *)&temp_v1_6->entityFlags & 0x4000)) {
                var_v0_3 = D_8009D1D4 + 1;
block_35:
                temp_v1_3 = var_v0_3 & 0xFF;
                temp_v1_3 <<= 3;
                temp_v1_3 = *(u16 *)((u8 *)D_800BE834 + temp_v1_3);
                D_8009D1D4 = var_v0_3;
                temp_v1_3 -= 3;
                temp_v1_3 = (u32) temp_v1_3 < 0x194U;
                if (!temp_v1_3 || ((u8) ((BattleEntity *)D254(14))->actionMode < 4U)) {
                    D1A0_W1 = D1A0_R1 & ~0x100;
                }
                Entity_SetActionMode(D254(15), ((Combatant *)D278(13))->actionMode12, var_a2, targetFound);
                *(s32 *)((char *)D278(10) + PE1_OFFSETOF(Combatant, stateFlags)) = (s32) (*(s32 *)((char *)D278(10) + PE1_OFFSETOF(Combatant, stateFlags)) | 0x200000);
                return;
            }
        }
block_39:
        Pm_StopAll();
        if (action == 6) {
            BattleEntity *attackTarget;
            queuedAction = ((Combatant *)D278(13))->action;
            if (!(*(s32 *)&queuedAction->attackWord & 0x3FF)) {
                BattleCmd_CommitAmmoAndUpdate(queuedAction);
            }
            *(s32 *)((char *)D278(10) + PE1_OFFSETOF(Combatant, stateFlags)) = (s32) (*(s32 *)((char *)D278(10) + PE1_OFFSETOF(Combatant, stateFlags)) | 0x100000);
            attackTarget = temp_s2->actor;
            temp_s0 = attackTarget->renderObject.target_y - D_8009D27C;
            temp_v0_2 = ratan2(temp_s0, Battle_CalcDistToPlayer(attackTarget, D254(14)));
            if (temp_v0_2 < -0xAB) {
                var_v1 = 0;
            } else {
                var_v1 = 2;
                if (temp_v0_2 < 0xE4) {
                    var_v1 = 1;
                }
            }
            Entity_SetActionMode(D254(15), *(u8 *)((char *)((void *)((s32)var_v1 + (s32)D278(13))) + PE1_OFFSETOF(Combatant, targetAngleActionModes)));
            *(s32 *)((char *)D278(10) + PE1_OFFSETOF(Combatant, stateFlags)) = (s32) (*(s32 *)((char *)D278(10) + PE1_OFFSETOF(Combatant, stateFlags)) & 0xFFDFFFFF);
            D_8009CE39 = 0x58;
            D_8009CE38 = 0;
            D_8009CE3A = 0x18;
            D_8009CE3B = 0;
        } else {
            Entity_SetActionMode(D254(14), 0xFU);
        }
        if (action != 0x13) {
            D_8009D258 = Scene_LoadRoomAssets(sp18.bytes[action], temp_s2->actor);
        }
        if (((u32) (action - 7) < 2U) || (action == 0xA)) {
            soundActor = temp_s2->actor;
            var_a0_3 = sp30.soundId[action];
            var_a2_2 = soundActor->renderObject.target_x;
            var_a3_2 = soundActor->renderObject.target_y;
            var_v0_5 = soundActor->renderObject.target_z;
        } else {
            var_a0_3 = sp30.soundId[action];
            var_a2_2 = ((BattleEntity *)D254(15))->posX.parts.integer;
            var_a3_2 = ((BattleEntity *)D254(15))->posY.parts.integer;
            var_v0_5 = ((BattleEntity *)D254(15))->posZ.parts.integer;
        }
        Akao_SendPositionalCmdStereo(var_a0_3, 0, var_a2_2, var_a3_2, var_v0_5);
        var_a1 = (void *)0xFFDFFFFF;
        var_a0_4 = D278(13);
        actor_flags = ((Combatant *)var_a0_4)->stateFlags;
        D1A0_W2 = D1A0_R2 | 0x100;
        actor_flags &= (s32)var_a1;
        ((Combatant *)var_a0_4)->stateFlags = actor_flags;
    }
block_55:
    actor = D278(10);
    actor_flags = actor->stateFlags;
    if (actor_flags & 0x80000) {
        if ((Battle_StepAyaAction() << 0x18) != 0) {
            end_slot = D_8009D1D4;
            aya_actor = D278(13);
            aya_player = D254(14);
            aya_actor->exp_or_acc = 0;
            *(s32 *)&aya_player->entityFlags = (s32) (*(s32 *)&aya_player->entityFlags & ~0x100);
            var_s0_4 = end_slot - 7;
            var_a2_2 = var_s0_4 & 0xFF;
            if ((u32)var_a2_2 < end_slot) {
                do {
                    var_v0_6 = var_a2_2 * 8;
                    temp_a0_4 = *(s32 *)(D_800BE830_BYTES + var_v0_6);
                    if (temp_a0_4 != D254(15)) {
                        Battle_UpdateEntityFacing(temp_a0_4);
                    }
                    var_s0_4 += 1;
                    var_a2_2 = var_s0_4 & 0xFF;
                } while ((u32)var_a2_2 < (u8) D_8009D1D4);
            }
            var_a0_4 = D_8009D20C;
            if (var_a0_4 != NULL) {
                do {
                    if (var_a0_4 != D_8009D254_10) {
                        temp_v0_4 = ((BattleEntity *)var_a0_4)->core;
                        if (temp_v0_4 != NULL) {
                            effect = temp_v0_4->effect;
                            if (effect != NULL) {
                                effect->state = 4;
                                temp_v1_8 = ((BattleEntity *)var_a0_4)->core;
                                *(s32 *)&temp_v1_8->coreFlags = (s32) (*(s32 *)&temp_v1_8->coreFlags & 0xC0FFFFFF);
                            }
                        }
                    }
                    var_a0_4 = ((BattleEntity *)var_a0_4)->next;
                } while (var_a0_4 != NULL);
            }
            *(s32 *)((char *)D278(10) + PE1_OFFSETOF(Combatant, stateFlags)) = (s32) (*(s32 *)((char *)D278(10) + PE1_OFFSETOF(Combatant, stateFlags)) & 0xFFF7FFFF);
            Battle_HaltOnPositiveX();
        }
    } else if (actor_flags & 0x100000) {
        temp_v0 = Battle_CalcAngleToTarget(&temp_s2->actor->renderObject, D254(14) + 0x28);
        ((BattleEntity *)D254(15))->facingAngle = temp_v0;
        Battle_StepCharacterAction(temp_s2);
        if ((u32) (*((D_8009D258 * 0xA0C) + D_800942E4) - 1) >= 2U) {
            var_a2_2 = 0xFFEFFFFF;
            temp_a0_3 = -0x101;
            var_a1 = D278(13);
            character_state = D254(13);
            ((Combatant *)var_a1)->exp_or_acc = 0;
            *(s32 *)&character_state->entityFlags = (s32) (*(s32 *)&character_state->entityFlags & temp_a0_3);
            system_flags = D1A0_R3;
            cleanup_flags.value = ((Combatant *)var_a1)->stateFlags;
            system_flags &= temp_a0_3;
            cleanup_flags.value &= var_a2_2;
            D1A0_W3 = system_flags;
            cleanup_flags.value |= 0x200000;
            ((Combatant *)var_a1)->stateFlags = cleanup_flags.value;
            Battle_HaltOnPositiveX();
        }
    } else {
        if (((BattleEntity *)D254(14))->animLastFrame == ((BattleEntity *)D254(14))->animPrev.parts.integer) {
            Entity_SetActionMode(D254(14), actor->actionMode12);
        }
        if ((((u32) (*((D_8009D258 * 0xA0C) + D_800942E4) - 1) >= 2U) || (temp_s2->field04 == 0x196)) && (((Combatant *)D278(13))->stateFlags = (s32) (((Combatant *)D278(13))->stateFlags | 0x200000), Battle_ApplySpellEffect(temp_s2->field04 - 0x183, temp_s2->actor), (temp_s2->field04 != 0x196))) {
            temp_v0_6 = D_8009D1D4 + 1;
            D_8009D1D4 = temp_v0_6;
            if (((u32) (D_800BE834[temp_v0_6 & 0xFF].value - 3) >= 0x194U) || ((u8) ((BattleEntity *)D254(15))->actionMode < 4U)) {
                D1A0_W4 = D1A0_R4 & ~0x100;
            }
            temp_a0_4 = (u8)D_8009CE3C;
            for (var_s0_4 = D_8009D1D4; (u32)(var_s0_4 & 0xFF) < (u32)temp_a0_4; var_s0_4++) {
                if ((u32)(D_800BE834[var_s0_4 & 0xFF].value - 1) < 2U) {
                    Battle_DispatchEntityEffect();
                    return;
                }
            }
        }
    }
}

void Battle_StartEnemyAttackEffect(BattleEntity *entity)
{
    Combatant *core = (Combatant *)entity->core;
    u32 *status = (u32 *)&core->statusFlags2;
    int effect;

    if ((ACTIVE->action->turnWord & 0x400) && ((*status & 3) != 1)) {
        effect = Scene_LoadRoomAssets(7, entity);
        D_8009D208 = effect;
        Pm_SendCmd(effect, 0, 0, 2, 0, 0);
        Asset_Find08Alt(0x484, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((ACTIVE->action->turnWord & 0x300) == 0x300 &&
        ((*status & 0x3C000) != 0x14000)) {
        D_8009D208 = Scene_LoadRoomAssets(90, entity);
        Asset_Find08Alt(0x488, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((ACTIVE->action->turnWord & 0x100) &&
        ((*status & 0xC000) != 0x4000)) {
        D_8009D208 = Scene_LoadRoomAssets(88, entity);
        Asset_Find08Alt(0x488, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((ACTIVE->action->turnWord & 0x200) &&
        ((*status & 0x30000) != 0x10000)) {
        D_8009D208 = Scene_LoadRoomAssets(89, entity);
        Asset_Find08Alt(0x486, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((ACTIVE->action->turnWord & 0x800) &&
        ((*status & 0xC) != 4)) {
        effect = Scene_LoadRoomAssets(7, entity);
        D_8009D208 = effect;
        Pm_SendCmd(effect, 0, 0, 1, 0, 0);
        Asset_Find08Alt(0x482, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((ACTIVE->action->turnWord & 0x1000) &&
        ((*status & 0x30) != 0x10)) {
        effect = Scene_LoadRoomAssets(7, entity);
        D_8009D208 = effect;
        Pm_SendCmd(effect, 0, 0, 0, 0, 0);
        Asset_Find08Alt(0x480, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
    }
}
