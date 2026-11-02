/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/battle_entity_anim.h"
#include "pe1/battle_reset_stats.h"
#include "pe1/battle_runtime.h"
#include "pe1/entity_animation.h"
#include "pe1/gte.h"
#include "pe1/math64.h"
#include "pe1/random.h"

/* Legacy name: this computes enemy damage, applies attack/status resistances,
 * subtracts HP, and starts the hit reaction. The double arithmetic calls the
 * LIBMATH soft-float entry points directly. Matching debt: the s4 pin keeps
 * enemy and its flagCore copy in retail's saved registers. */
void Battle_StepEnemyMovement(BattleEntity *entity)
{
    BattleAction *weaponAction;
    BattleAction *defenseAction;
    BattleAction *activeAction;
    EnemyCombatant *flagCore;
    register EnemyCombatant *enemy asm("$20");
    s32 basePower;
    s32 disableResistance;
    s32 activeState;
    s32 hitReaction;
    s32 attackClass;
    s32 firstResistance;
    s32 secondResistance;
    s32 poisonResistance;
    s32 stopResistance;
    s32 attackPower;
    s32 damage;
    register unsigned char resistanceMode;
    u32 *statusFlags;
    u32 flags;
    u32 *turnFlags;
    enemy = entity->core;
    statusFlags = &enemy->statusFlags2;
    activeState = g_ActiveActor->stateFlags;
    flagCore = enemy;
    if (activeState & 0x80000)
    {
        register s32 stat = (u16)g_ActiveActor->stat1E;
        double base, scale;
        /* Integer bit patterns avoid native GCC's broken double literal pool. */
        union { double value; unsigned long long bits; } factor;
        stat -= 25;
        stat += g_ActiveActor->field04.fieldId04;
        base = __floatsidf(stat * 6);
        scale = __floatsidf(g_BattleTargetCount - 1);
        factor.bits = 0x3FB999999999999AULL; /* 0.1 */
        scale = __muldf3(scale, factor.value);
        scale = __adddf3(scale, 1.0);
        scale = __muldf3(base, scale);
        factor.bits = 0x401C000000000000ULL; /* 7.0 */
        attackPower = __fixdfsi(__divdf3(scale, factor.value));
    }
    else if (activeState & 0x100000)
    {
        basePower = (((u16)g_ActiveActor->stat1E / 5U) & 0xFFFF) + (s16) g_ActiveActor->action->powerBonus;
        attackPower = ((basePower * (((g_ActiveActor->field04.fieldId04 * 7) + 0x78) / 160) * (s16)(g_ActiveActor->exp_or_acc >> 16)) / (s16)(g_ActiveActor->maxAtk >> 16)) + basePower;
    }
    else
    {
        unsigned char hitCountPercent[11] = { 0, 100, 60, 41, 0, 25, 0, 18, 0, 0, 13 };
        int strength;
        weaponAction = g_ActiveActor->action;
        strength = (u16)g_ActiveActor->stat1E / 5U;
        attackPower = ((strength + (s16) weaponAction->powerBonus) * hitCountPercent[weaponAction->turnWord & 0xF]) / 100;
    }
    if (!(g_ActiveActor->stateFlags & 0x180000) && (defenseAction = g_ActiveActor->action, (defenseAction->actionCode.actionId != 8)))
    {
        int remainingActions = defenseAction->turnWord & 0xF;
        damage = attackPower - (enemy->defense / remainingActions);
    }
    else
    {
        damage = attackPower - enemy->defense;
    }
    if (damage > 0)
    {
        if (!(g_ActiveActor->stateFlags & 0x180000))
        {
            activeAction = g_ActiveActor->action;
            turnFlags = &activeAction->turnWord;
            hitReaction = flagCore->coreFlags & 0x38000;
            resistanceMode = 0;
            if (hitReaction == 0x8000) damage = damage * 3 / 10;
            else if (hitReaction == 0x18000) damage = damage * 3 / 2;
            else if (hitReaction == 0x20000) damage /= 10;
            attackClass = (flagCore->coreFlags >> 0x12) & 3;
            if (attackClass != 2)
            {
                if (attackClass == 3)
                {
                    (*statusFlags) |= 0x01000000;
                }
                else
                {
                if (*turnFlags & 0x400)
                {
                    disableResistance = (*statusFlags) & 3;
                    switch (disableResistance)
                    {
                    case 0:
                        if (!(rand() & 1))
                        {
                        case 2:
                            flagCore->coreFlags |= 0x400;
                        }
                        break;
                    }
                }
                if (!(flagCore->coreFlags & 0x400))
                {
                    if (*turnFlags & 0x100)
                    {
                        firstResistance = ((*statusFlags) >> 0xE) & 3;
                        switch (firstResistance)
                        {
                            case 1: resistanceMode = 1;
                            break;
                            case 2: resistanceMode = 2;
                            break;
                        }
                    }
                    if (((resistanceMode & 0xFF) != 2) && (*turnFlags & 0x200))
                    {
                        secondResistance = (u16)(*statusFlags >> 16) & 3;
                        switch (secondResistance)
                        {
                        case 1:
                            resistanceMode = 1;
                            break;
                        case 2:
                            resistanceMode = 2;
                            break;
                        }
                    }
                    if (*turnFlags & 0x800)
                    {
                        poisonResistance = ((*statusFlags) >> 2) & 3;
                        switch (poisonResistance)
                        {
                        case 0:
                            if (!(rand() & 1))
                            {
                            case 2:
                                flagCore->coreFlags = (flagCore->coreFlags | 0x10) & ~0x3E0;
                            }
                            break;
                        }
                    }
                    if (*turnFlags & 0x1000)
                    {
                        stopResistance = ((*statusFlags) >> 4) & 3;
                        switch (stopResistance)
                        {
                        case 0:
                            if (!(rand() & 1))
                            {
                            case 2:
                                flagCore->coreFlags |= 0x1800;
                                entity->entityFlags |= 0x1000;
                                if (enemy->field04.bytes.kind != 0)
                                {
                                    Entity_SetActionMode(entity, (u16)(s8)enemy->lowAndEntityId.bytes.low);
                                }
                            }
                            break;
                        }
                    }
                }
                }
            }
            switch (resistanceMode & 0xFF)
            {
            case 1:
                damage /= 5;
                break;
            case 2:
                damage = (damage * 3) / 2;
                break;
            }
        }
    }
    if (damage <= 0)
    {
        flags = flagCore->coreFlags;
        damage = 1;
        if ((flags & 0x38000) == 0x18000)
        {
            damage = 2;
        }
        if ((flags & 0xC0000) == 0xC0000)
        {
            (*statusFlags) |= 0x01000000;
        }
    }
    enemy->hpAlive -= damage;
    if (!(g_ActiveActor->stateFlags & 0x80000))
    {
        Battle_UpdateEntityFacing(entity);
    }
    flagCore->coreFlags = (flagCore->coreFlags & ~0x6000) | 0x4000;
}

typedef struct FacingMotion {
    u8 state;
    u8 phase;
    u16 amplitude;
    s16 angle;
} FacingMotion;

typedef struct FacingCore {
    u32 flags;
    u8 pad04;
    s8 status05;
    u8 pad06[0x16];
    u8 actions[0x80];
    u8 pad9C[8];
    FacingMotion motion;
    u8 padAA[0x12];
    u8 mode;
    u8 previous_mode;
    u8 step_flag;
    u8 padBF;
    s32 previous_frame;
    s32 previous_step;
    s32 previous_delta;
} FacingCore;

int Battle_CalcRelativeAngle(BattleEntity *, BattleEntity *);

void Battle_UpdateEntityFacing(BattleEntity *entity)
{
    FacingCore *core = entity->core;
    s16 direction;
    int mode;
    int phase;
    int value;
    FacingMotion *motion;

    if (core->status05 != 0 || (core->flags & 0xE)) return;
    if (!((g_ActiveActor->stateFlags & 0x80000) && core->actions[(core->flags >> 17) & 0x70])) {
        if (core->mode == 1) {
            core->mode++;
            core->previous_mode = entity->actionMode;
            core->step_flag = (entity->entityFlags >> 9) & 1;
            core->previous_frame = entity->animFrame;
            core->previous_step = entity->animPrev.fixed;
            core->previous_delta = entity->animStep;
        }
    }
    direction = (s16)Battle_CalcRelativeAngle(D_8009CD10, entity);
    if (direction > 0x400) {
        if (direction < 0xC00) {
            Entity_SetActionMode(entity, 1);
        } else {
            Entity_SetActionMode(entity, 0);
        }
    } else {
        Entity_SetActionMode(entity, 0);
    }
    entity->animStep = 0x10000;
    entity->entityFlags |= 0x1000;
    if (g_ActiveActor->stateFlags & 0x80000) return;
    if (!(core->flags & 0x100000)) return;
    if (core->motion.state == 0) return;
    motion = &core->motion;
    switch (core->motion.state) {
    case 2:
        core->motion.phase = 2;
        core->motion.amplitude = 0x46;
        break;
    case 3:
        core->motion.phase = 5;
        core->motion.amplitude = 0x14;
        break;
    case 1:
        core->motion.amplitude = 0x190;
        break;
    }
    phase = (u16)D_8009CD10->facingAngle + 0x800;
    motion->angle = phase;
    value = rsin((s16)phase);
    entity->posX.fixed += (motion->amplitude * value) << 4;
    value = rcos(motion->angle);
    entity->posZ.fixed += (motion->amplitude * value) << 4;
}

void Battle_StepEntityAnimState(BattleEntity *entity) {
    EnemyCombatant *state;
    BattleEntity *iter;
    u8 color;
    int product;
    int diff;
    int i;
    u8 all_done;

    state = entity->core;

    switch (state->deathAnimPhase) {
    case 0:
        g_BattleLiveEnemyCount--;
        state->deathAnimPhase++;
        /* fallthrough */
    case 1:
        if (state->deathPersist != 0) {
            if ((u16)(entity->animFrame >> 16) < state->deathAnimFrame &&
                entity->actionMode == 0 &&
                state->field04.bytes.kind < 2) {
                return;
            }

            entity->entityFlags |= 0x100;
            Anim_SetInterpRate(&entity->renderObject, 0x16);
            state->deathFadeStep = 0;
            state->deathAnimPhase++;
            entity->motionX = 0;
            entity->motionY = 0;
            entity->motionZ = 0;
            entity->entityFlags |= 0x1000;
            Akao_Cmd_21(0, state->slotBit);

            iter = (BattleEntity *)g_FieldActorListHead;
            while (iter != 0) {
                if (iter->core == 0 && iter->parent == entity) {
                    Anim_SetInterpRate(&iter->renderObject, 0x16);
                }
                iter = iter->next;
            }
        } else {
            Akao_Cmd_21(0, state->slotBit);
            state->deathAnimPhase = 3;
        }
        break;

    case 2:
        color = (-0x80 - (state->deathFadeStep << 4)) & 0xFF;
        Render_FadeEntityColor(&entity->renderObject, color, 0x80, color);
        if (color == 0) {
            entity->renderObject.flags_9C |= 2;
            state->deathFadeStep = 0;
            state->deathAnimPhase++;
        } else {
            if (state->deathFadeStep == 0 && state->deathAssetEnabled != 0) {
                Asset_Find08w(
                    state->deathAssetId,
                    0,
                    entity->renderObject.target_x,
                    entity->renderObject.target_y,
                    entity->renderObject.target_z);
            }
            state->deathFadeStep++;
        }

        iter = (BattleEntity *)g_FieldActorListHead;
        while (iter != 0) {
            if (iter->core == 0 && iter->parent == entity) {
                Render_FadeEntityColor(&iter->renderObject, color, 0x80, color);
                if (color == 0) {
                    iter->renderObject.flags_9C |= 2;
                }
            }
            iter = iter->next;
        }
        break;

    case 3:
        if (entity->renderObject.variant_visible != 0 && state->deathPersist != 0) {
            return;
        }

        entity->entityFlags |= 0x410;
        Battle_SlotFree(entity);

        iter = (BattleEntity *)g_FieldActorListHead;
        while (iter != 0) {
            if (iter->core == 0 && iter->parent == entity) {
                iter->entityFlags |= 0x10;
            }
            iter = iter->next;
        }

        product = state->rewardFactorA * state->rewardFactorB;
        diff = state->rewardBase - product;
        g_BattleExpReward += state->hpMirror;
        if (diff > 0) {
            g_BattlePeBonusReward += diff;
        }

        for (i = 0; (u8)i < 10; i++) {
            if (g_BattleRewardSlots[(u8)i].id == 0) {
                g_BattleRewardSlots[(u8)i].id = state->rewardSlotId;
                break;
            }
        }

        if (state->lootItemId != 0) {
            for (i = 0; (u8)i < 10; i++) {
                if (g_BattleRewardSlots[(u8)i].id == 0) {
                    g_BattleRewardSlots[(u8)i].id = (u16)state->lootItemId;
                    if (state->lootItemAux >= 0) {
                        g_BattleRewardSlots[(u8)i].extra = state->lootItemAux;
                    }
                    break;
                }
            }
        }

        if (g_BattleLiveEnemyCount != 0) {
            return;
        }

        iter = (BattleEntity *)g_FieldActorListHead;
        all_done = 1;
        while (iter != 0) {
            if (iter != D_8009CD10 && iter->core != 0) {
                all_done = 0;
            }
            iter = iter->next;
        }

        if (all_done && (s16)g_ActiveActor->curHP > 0) {
            Battle_StartDeathAnim();
        }
        break;
    }
}
