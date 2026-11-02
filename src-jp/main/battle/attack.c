/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/akao.h"
#include "pe1/battle.h"
#include "pe1/battle_ailment.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_main.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_word_records.h"
#include "pe1/entity_animation.h"
#include "pe1/field_movement.h"
#include "pe1/gte_types.h"
#include "pe1/inventory.h"
#include "pe1/item_ids.h"
#include "pe1/random.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
u8 g_BattleActionAnimModes[4];
u8 g_BattleActionQueueCount;
u8 g_BattleActionIndex;
u8 g_BattleRemainingActions;
int g_BattleAttackPmSlot;
u8 g_BattleHitTimer;
s16 g_BattlePlayerAimHeight;
u8 g_BattleHitPending;
u8 g_AyaActionStep;
s8 g_AyaAttackStep;
BattleEvasionOutcome g_EvasionOutcome;
s16 g_AyaActionTimer;
BattleSavedRotation g_AyaSavedRotation;
int g_BattleEffectPmSlot;

void Battle_FinalizeAttackResult(void)
{
    Combatant *combatant;
    BattleAction *action;
    u32 attack_word;

    combatant = g_ActiveActor;
    action = combatant->action;

    if (action->actionCode.actionId == 8) {
        BattleEntity *player;

        g_BattleHitPending = 1;
        player = D_8009CD10;
        Asset_Find08Alt(0x46C, 0, player->posX.parts.integer,
                        player->posY.parts.integer, player->posZ.parts.integer);
        return;
    }

    attack_word = action->attackWord;
    if ((attack_word & 0x3FF) != 0) {
        g_BattleHitPending = 1;
        if (!(combatant->stateFlags & 0x100000)) {
            BattleEntity *player;

            Pm_SendCmd(g_BattleAttackPmSlot, 0, 0, 1, 0, 0);
            player = D_8009CD10;
            Akao_SetPos3D(0, 0, player->posX.parts.integer,
                          player->posY.parts.integer, player->posZ.parts.integer);
        }

        {
            Combatant *next_combatant;
            BattleAction *next_action;
            u32 next_attack_word;

            next_combatant = g_ActiveActor;
            next_action = next_combatant->action;
            next_attack_word = next_action->attackWord;
            next_action->attackWord = (next_attack_word & ~0x3FF) |
                                      (((next_attack_word & 0x3FF) - 1) & 0x3FF);
        }
        return;
    }

    {
        BattleEntity *player;

        g_BattleHitPending = 0;
        player = D_8009CD10;
        Asset_Find08Alt(0x46E, 0, player->posX.parts.integer,
                        player->posY.parts.integer, player->posZ.parts.integer);
    }
}

int BattleCmd_CommitAmmoAndUpdate(); /* no prototype: retail calls it with and without the action */

int Battle_StepCharacterAction(BattleInitSlot *slot)
{
    BattleInitSlot * turnSlot = slot;
    int result = 0;
    register BattleAction *activeAction asm("$4");
    Combatant * active;
    BattleEntity * caseEntity;
    BattleEntity *idleEntity;
    Combatant *idleActor;
    int retryCount;
    int indexB;
    register int nextIndexB asm("$3");
    int indexAlt;
    int indexDefault;
    BattleEntity **actorField;
    int angle;
    int direction;
    register int predicate asm("$2");
    register int facingAngle asm("$4");
    register int height asm("$16");

    switch (D_8009CD10->actionMode) {
    case 6:
    case 8:
    case 10: {
        BattleEntity *current = D_8009CD10;
        if (current->animLastFrame != ((u16 *)&current->animFrame)[1])
            break;
        if (g_BattleActionAnimModes[0] != 0) {
            g_BattleActionAnimModes[0]--;
        } else if (g_BattleActionAnimModes[1] != 0) {
            g_BattleActionAnimModes[1]--;
        } else {
            goto ready;
        }
        current->entityFlags |= 0x100;
        break;
    ready:
        }
        g_BattleHitTimer = 0;
        actorField = &turnSlot->actor;
        { u8 animMode = g_ActiveActor->action->animMode[1]; g_BattleActionAnimModes[1] = animMode; }
        { height = (*actorField)->renderObject.target_y; angle = height - g_BattlePlayerAimHeight; }
        { facingAngle = ratan2(angle, Battle_CalcDistToPlayer(turnSlot->actor, D_8009CD10));
        if (facingAngle < -0xAB) direction = 0; else { predicate = facingAngle < 0xE4; if (predicate) direction = 1; else direction = 2; } }
        Entity_SetActionMode(D_8009CD10, *(direction + g_ActiveActor->followUpAngleActionModes));
        D_8009CD10->entityFlags &= ~0x100;
        g_BattleRemainingActions = ((BattleTurnBits *)&g_ActiveActor->action->turnWord)->actions;
        Battle_FinalizeAttackResult();
        break;

    case 7:
    case 9:
    case 11:
        caseEntity = D_8009CD10;
        if (caseEntity->animLastFrame != caseEntity->animPrev.parts.integer)
            break;
        indexB = g_BattleActionIndex;
        if (g_BattleActionQueue[indexB].command == 0x189) {
            idleActor = g_ActiveActor;
            idleEntity = D_8009CD10;
        }
        else
        {
        { active = g_ActiveActor;
        activeAction = active->action;
        if ((activeAction->turnWord & 0xC0) == 0xC0) {
            int pending = g_BattleRemainingActions;
            int narrow;
            predicate = 1;
            asm volatile("" : "=r"(pending) : "0"(pending));
            narrow = (u8)pending;
            retryCount = pending;
            if (narrow != predicate) goto alternate;
        }
        nextIndexB = indexB + 1;
        { register int offset asm("$5") = nextIndexB << 3;
        { int nextCode = g_BattleActionQueue[nextIndexB].command;
        g_BattleRemainingActions = 0;
        if (nextCode >= 3 || nextIndexB == g_BattleActionQueueCount) {
            Entity_SetActionMode(D_8009CD10, active->actionMode12);
            g_ActiveActor->stateFlags |= 0x200000;
            { u8 animMode = g_ActiveActor->action->animMode[0]; g_BattleActionAnimModes[0] = animMode; }
            result = 1; break;
        }
        }
        { EnemyCombatant *enemy;
        { BattleInitSlotAddress at; at.slot = g_BattleActionQueue; at.address = at.address + offset; enemy = at.slot->actor->core; }
        if (!(enemy->hpAlive > 0 && enemy)) {
            Entity_SetActionMode(D_8009CD10, active->actionMode12);
            g_BattleActionIndex++;
            g_ActiveActor->stateFlags |= 0x200000;
            goto action_done;
        }
        }
        }
        }
        { activeAction = active->action;
        if (activeAction->attackWord & 0x3FF) {
            { height = turnSlot->actor->renderObject.target_y; angle = height - g_BattlePlayerAimHeight; }
            { facingAngle = ratan2(angle, Battle_CalcDistToPlayer(turnSlot->actor, D_8009CD10));
        if (facingAngle < -0xAB) direction = 0; else { predicate = facingAngle < 0xE4; if (predicate) direction = 1; else direction = 2; } }
            Entity_SetActionMode(D_8009CD10, *(direction + g_ActiveActor->targetAngleActionModes));
            result = 1;
            { register Combatant *postActor = g_ActiveActor;
              { register BattleEntity *postEntity = D_8009CD10;
                postEntity->animFrame = D_8009CD10->animLastFrame << 16;
                D_8009CD10->entityFlags |= 0x100; }
              { register BattleAction *postAction = postActor->action;
                { u32 turnWord = postAction->turnWord;
                g_BattleActionAnimModes[1] += g_BattleActionAnimModes[2];
                g_BattleRemainingActions = turnWord & 0xF; } } }
            break;
        }
        }
        if (BattleCmd_CommitAmmoAndUpdate()) {
            Entity_SetActionMode(D_8009CD10, 0xC);
            Akao_SetPos3D(1, 0, D_8009CD10->posX.parts.integer,
                          D_8009CD10->posY.parts.integer, D_8009CD10->posZ.parts.integer);
        action_done:
            result = 1; asm volatile(""); break;
        }
        result = 1; break;

    alternate:
        if (activeAction->attackWord & 0x3FF) {
            indexAlt = g_BattleActionIndex;
            {
                EnemyCombatant *enemy = g_BattleActionQueue[indexAlt].actor->core;
                int nextRetry;
                if (enemy->hpAlive > 0) {
asm volatile("" : : "r"(enemy));
                nextRetry = retryCount - 1;
                if (enemy) goto target_alive;
                }
            target_dead:
            Entity_SetActionMode(caseEntity, active->actionMode12);
            g_ActiveActor->stateFlags |= 0x200000;
            g_BattleRemainingActions = ((BattleTurnBits *)&g_ActiveActor->action->turnWord)->actions;
            result = 1; break;
            target_alive:
            g_BattleRemainingActions = nextRetry;
            Battle_FinalizeAttackResult();
            break;
            }
        }
        if (BattleCmd_CommitAmmoAndUpdate()) {
            Entity_SetActionMode(D_8009CD10, 0xC);
            Akao_SetPos3D(1, 0, D_8009CD10->posX.parts.integer,
                          D_8009CD10->posY.parts.integer, D_8009CD10->posZ.parts.integer);
            break;
        }
        idleActor = g_ActiveActor;
        idleEntity = D_8009CD10;
        }
    set_idle:
        result = 1;
        Entity_SetActionMode(idleEntity, idleActor->actionMode12);
        break;
    default:
        indexDefault = g_BattleActionIndex;
        if (g_BattleActionQueue[indexDefault].command == 0x189)
            break;
        g_BattleActionIndex = indexDefault + 1;
        if ((u8)(indexDefault + 1) != g_BattleActionQueueCount)
            break;
        if (g_BattleAttackPmSlot < 0)
            break;
        if (g_ActiveActor->action->actionCode.actionId != 8)
            Pm_SendCmd(g_BattleAttackPmSlot, 0, 0, 2, 0, 0);
        Pm_SendCmd(g_BattleImpactPmSlot, 0, 0, 2, 0, 0);
        break;
    }
    return result;
}

/* COMMON supplies small-data metadata to stock MASPSX. Existing retail
 * data/linker symbols provide storage; these do not allocate new globals. */
u8 g_BattleHitTimer, g_BattleActionIndex, g_BattleHitPending;
s8 g_EvasionReaction;

/* Apply the evasion outcome Battle_CheckEvasion left in g_EvasionOutcome and
 * g_EvasionReaction to one target: a resolved hit sets the hit state, the
 * weapon category and the reaction in the core flags; an unresolved one
 * opens the floating panel.
 * The steal path reaches this through jump cross-jumping into the
 * single-target copy, as in retail. */
static inline void ApplyEvasionOutcome(BattleEntity *target)
{
    EnemyCombatant *core = target->core;
    if (g_EvasionOutcome.status == 1)
    {
        EnemyCoreFlagBits *bits = (EnemyCoreFlagBits *)&core->coreFlags;
        bits->hitState = 1;
        bits->category = g_ActiveActor->action->attackWord >> 20;
        bits->reaction = ((BattleEvasionReaction *)&g_EvasionReaction)->value;
    }
    else if (g_EvasionOutcome.status == 0)
    {
        core->panelC_val = -1;
        core->panelC_timer = 0x1E;
        core->panelC_x = target->renderObject.projected_target_x;
        core->panelC_y = target->renderObject.projected_target_y - 0x14;
    }
}

/* Resolve the timed hit for stealing, chained targets, cone attacks or a single
 * target. */
void Battle_ResolveHitOnTimer(void)
{
    BattleAction *action;
    BattleInitSlot *selectedSlot;
    BattleEntity *target;
    s16 actionId;
    s32 stealMode;
    u8 hitTimer;
    hitTimer = g_BattleHitTimer + 1;
    action = g_ActiveActor->action;
    selectedSlot = &g_BattleActionQueue[g_BattleActionIndex];
    g_BattleHitTimer = hitTimer;
    if ((hitTimer & 0xFF) != action->hitTrigger)
        return;
    actionId = action->actionCode.actionId;
    if (actionId == 8)
    {
        Battle_CheckEvasion(selectedSlot->actor, &g_EvasionOutcome, 0);
        if (g_EvasionOutcome.status == 1)
        {
            BattleEntity *thief = selectedSlot->actor;
            Asset_Find08Alt(0x46D, 0, thief->renderObject.target_x, thief->renderObject.target_y, thief->renderObject.target_z);
            if (g_ActiveActor->action->turnWord & 0x6000)
            {
                stealMode = (((EnemyCombatant *)selectedSlot->actor->core)->statusFlags2 >> 0xC) & 3;
                switch (stealMode)
                {
                    /* Retail switch shares the steal body with case 0. */
                case 0:
                    if (!(rand() & 1))
                    {
                    case 2:
                        if (Inv_AddItem(((EnemyCombatant *)selectedSlot->actor->core)->stealItemId) != 0)
                        {
                            EnemyCombatant *enemy = selectedSlot->actor->core;
                            g_BattleSaveOverlayActive = 1;
                            g_CurItemEffectData = Inv_GetItemEffectData(enemy->stealItemId, 2);
                        }
                        else
                        {
                            g_BattleSaveOverlayActive = 1;
                            g_CurItemEffectData = Inv_GetItemEffectData(((EnemyCombatant *)selectedSlot->actor->core)->stealItemId, 1);
                            ((EnemyCombatant *)selectedSlot->actor->core)->stealItemId = 0;
                        }
                    }
                    break;
                }
            }
        }
        ApplyEvasionOutcome(selectedSlot->actor);
    }
    else if (actionId == 6)
    {
        s16 reaction;
        Battle_CheckEvasion(selectedSlot->actor, &g_EvasionOutcome, 0);
        ApplyEvasionOutcome(selectedSlot->actor);
        reaction = g_EvasionReaction;
        if (g_EvasionOutcome.status == 1)
        {
            for (target = (BattleEntity *)g_FieldActorListHead; target != 0; target = target->next)
            {
                EnemyCombatant *core;
                u32 flags;
                if (target == D_8009CD10)
                    continue;
                core = target->core;
                if (core == 0)
                    continue;
                flags = target->entityFlags;
                if (((flags & 0x2040) != 0x40) && !(flags & 0x4000) && (core->hpAlive > 0) && (target != selectedSlot->actor))
                {
                    Battle_CheckEvasion(target, &g_EvasionOutcome, reaction);
                    ApplyEvasionOutcome(target);
                }
            }
        }
    }
    else if (((action->turnWord & 0xC0) == 0x80) && !(g_ActiveActor->stateFlags & 0x100000))
    {
        u16 playerAngle = D_8009CD10->facingAngle;
        s16 centered;
        s32 lowerBound;
        s32 upperBound;
        upperBound = playerAngle - 0x800;
        centered = upperBound;
        if (centered < -0x600)
        {
            upperBound = playerAngle + 0x600;
            lowerBound = playerAngle - 0x600;
        }
        else if (centered < 0x600)
        {
            lowerBound = playerAngle - 0xA00;
            upperBound = playerAngle - 0x600;
        }
        else
        {
            lowerBound = playerAngle - 0x1600;
            upperBound = playerAngle - 0xA00;
        }
        target = (BattleEntity *)g_FieldActorListHead;
        if (target != 0)
        {
            s32 upper = (s16)upperBound;
            s32 lower = (s16)lowerBound;
            s32 facing = centered;
            int upperRange = facing < 0x600;
            do
            {
                BattleEntity *player = D_8009CD10;
                EnemyCombatant *core;
                u32 flags;
                s32 angle;
                if (target == player)
                    continue;
                core = target->core;
                if (core == 0)
                    continue;
                flags = target->entityFlags;
                if (((flags & 0x2040) == 0x40) || (flags & 0x4000) || (core->hpAlive <= 0))
                    continue;
                angle = ratan2(target->renderObject.target_x - player->posX.parts.integer, target->renderObject.target_z - player->posZ.parts.integer);
                if (facing >= -0x600 && upperRange)
                {
                    if ((s16)angle < lower || upper < (s16)angle)
                        continue;
                }
                else
                {
                    if ((s16)angle < upper && lower < (s16)angle)
                        continue;
                }
                Battle_CheckEvasion(target, &g_EvasionOutcome, 0);
                ApplyEvasionOutcome(target);
            } while ((target = target->next) != 0);
        }
    }
    else if (selectedSlot->actor != 0)
    {
        Battle_CheckEvasion(selectedSlot->actor, &g_EvasionOutcome, 0);
        ApplyEvasionOutcome(selectedSlot->actor);
    }
    g_BattleHitPending = 0;
    g_BattleHitTimer = 0;
}

void Aya_DeriveStats(int *first, int *second);
void Draw_SetupSprites(BattleEntity *actor, int unused, int kind);

void Battle_ApplyDamage(int damageKind) {
    int effect;
    s8 result = -1;
    Combatant *actor;
    int flags;

    if ((g_GameStateFlags & 2) == 0) {
        BattleEntity *localActor = D_8009CD10;
        g_ActiveActor = localActor->core;
        effect = damageKind < ITEM_CURE_P ? 0x4B4 : 0x4B5;
        Asset_Find08Alt(effect, 1, localActor->posX.parts.integer,
                        localActor->posY.parts.integer,
                        localActor->posZ.parts.integer);
    } else {
        BattleEntity *localActor;
        effect = damageKind < ITEM_CURE_P ? 0x4B4 : 0x4B5;
        localActor = D_8009CD10;
        Asset_Find08Alt(effect, 0, localActor->posX.parts.integer,
                        localActor->posY.parts.integer,
                        localActor->posZ.parts.integer);
    }

    switch (damageKind) {
    case ITEM_MEDICINE_1:
        g_ActiveActor->curHP += 0x2D;
        break;
    case ITEM_MEDICINE_2:
        g_ActiveActor->curHP += 0x5A;
        break;
    case ITEM_MEDICINE_3:
        g_ActiveActor->curHP += 0xB4;
        break;
    case ITEM_MEDICINE_4:
        g_ActiveActor->curHP += 0x190;
        break;
    case ITEM_FULL_RECOVER:
        g_ActiveActor->curHP = g_ActiveActor->maxHP;
        break;
    case ITEM_SUPER_JUNK:
        g_ActiveActor->exp_or_acc += g_ActiveActor->maxAtk / 4;
        break;
    case ITEM_DUPER_JUNK:
        g_ActiveActor->exp_or_acc += g_ActiveActor->maxAtk / 2;
        break;
    case ITEM_CURE_P: {
        Combatant *target = g_ActiveActor;
        int field = target->stateFlags;
        if ((field & AYA_POISON_MASK) == 0) {
            target->stateFlags = field | AYA_POISON_IMMUNE;
        } else if ((field & AYA_POISON_MASK) != AYA_POISON_IMMUNE) {
            target->stateFlags = field & ~AYA_POISON_MASK;
        }
        result = 7;
        break;
    }
    case ITEM_CURE_D: {
        Combatant *target = g_ActiveActor;
        int field = target->stateFlags;
        if ((field & AYA_DARKNESS_MASK) == 0) {
            target->stateFlags = field | AYA_DARKNESS_IMMUNE;
        } else if ((field & AYA_DARKNESS_MASK) != AYA_DARKNESS_IMMUNE) {
            target->stateFlags = field & ~AYA_DARKNESS_MASK;
        }
        result = 5;
        break;
    }
    case ITEM_CURE_C: {
        Combatant *target = g_ActiveActor;
        int field = target->stateFlags;
        if ((field & AYA_CONFUSION_MASK) == 0) {
            target->stateFlags = field | AYA_CONFUSION_IMMUNE;
        } else if ((field & AYA_CONFUSION_MASK) != AYA_CONFUSION_IMMUNE) {
            target->stateFlags = field & ~AYA_CONFUSION_MASK;
            D_8009CDA8 &= ~FIELD_MOVE_CONFUSED;
        }
        result = 4;
        break;
    }
    case ITEM_CURE_M: {
        Combatant *target = g_ActiveActor;
        int field = target->stateFlags;
        if ((field & AYA_STIFFNESS_MASK) == 0) {
            target->stateFlags = field | AYA_STIFFNESS_IMMUNE;
        } else if ((field & AYA_STIFFNESS_MASK) != AYA_STIFFNESS_IMMUNE) {
            target->stateFlags = field & ~AYA_STIFFNESS_MASK;
        }
        result = 6;
        break;
    }
    case ITEM_FULL_CURE:
        actor = g_ActiveActor;
        Aya_DeriveStats(&actor->atbStep, &actor->atbRate);
        actor = g_ActiveActor;
        flags = actor->stateFlags;
        if ((flags & AYA_POISON_MASK) != AYA_POISON_IMMUNE) {
            actor->stateFlags = flags & ~AYA_POISON_MASK;
        }
        actor = g_ActiveActor;
        flags = actor->stateFlags;
        if ((flags & AYA_CONFUSION_MASK) != AYA_CONFUSION_IMMUNE) {
            actor->stateFlags = flags & ~AYA_CONFUSION_MASK;
            D_8009CDA8 &= ~FIELD_MOVE_CONFUSED;
        }
        actor = g_ActiveActor;
        flags = actor->stateFlags;
        if ((flags & AYA_DARKNESS_MASK) != AYA_DARKNESS_IMMUNE) {
            actor->stateFlags = flags & ~AYA_DARKNESS_MASK;
        }
        actor = g_ActiveActor;
        flags = actor->stateFlags;
        if ((flags & AYA_STIFFNESS_MASK) != AYA_STIFFNESS_IMMUNE) {
            actor->stateFlags = flags & ~AYA_STIFFNESS_MASK;
        }
        g_ActiveActor->stateFlags &= ~0x1000;
        break;
    }

    {
        Combatant *endActor = g_ActiveActor;
        if ((s16)endActor->maxHP < (s16)endActor->curHP) {
            endActor->curHP = (s16)endActor->maxHP;
        }
    }
    if (result != -1 && (g_GameStateFlags & 2)) {
        Draw_SetupSprites(D_8009CD10, 0, (u8)result);
    }
}

s32 rand(void);
u16 g_ActorEffectFlag100Timer;
void *g_BattleActiveEntity;

/* The acting combatant, reloaded after every structure store as retail does. */
#define ACTOR g_ActiveActorRecord.actor

/* Applies Parasite Energy technique idx (command - PE_SPELL_COMMAND_BASE) cast
 * by the player on target ent, then charges its PE cost. */
void Battle_ApplySpellEffect(u32 idx, BattleEntity *ent) {
    BattleActionSoundTable soundTable;
    s32 dmg;

    dmg = 0;
    if (!(g_GameStateFlagsRecord.flags & 2)) {
        soundTable = g_BattleActionSoundIds;
        Akao_SendPositionalCmdStereo(soundTable.soundId[idx], 1, g_PlayerEntityRecord.entity->posX.parts.integer, g_PlayerEntityRecord.entity->posY.parts.integer, g_PlayerEntityRecord.entity->posZ.parts.integer);
        ACTOR = g_PlayerEntityRecord.entity->core;
    }
    switch (idx) {
    case PE_SPELL_HEAL_1:
        dmg = 0x3C0000;
        ACTOR->curHP += 0x1E;
        break;
    case PE_SPELL_HEAL_2:
        dmg = 0x780000;
        ACTOR->curHP += 0x3C;
        break;
    case PE_SPELL_HEAL_3:
        dmg = 0x01F40000;
        ACTOR->curHP += 0x118;
        break;
    case PE_SPELL_DETOX:
        if ((ACTOR->stateFlags & AYA_POISON_MASK) != AYA_POISON_MASK) {
            ACTOR->stateFlags &= ~AYA_POISON_MASK;
        }
        dmg = 0x640000;
        break;
    case PE_SPELL_MEDIC:
        /* Medic: cures every ailment and the 0x1000 state. */
        if ((ACTOR->stateFlags & AYA_POISON_MASK) != AYA_POISON_MASK) {
            ACTOR->stateFlags &= ~AYA_POISON_MASK;
        }
        if ((ACTOR->stateFlags & AYA_CONFUSION_MASK) != AYA_CONFUSION_MASK) {
            ACTOR->stateFlags &= ~AYA_CONFUSION_MASK;
            g_FieldMoveLockRecord.flags &= ~FIELD_MOVE_CONFUSED;
        }
        if ((ACTOR->stateFlags & AYA_DARKNESS_MASK) != AYA_DARKNESS_MASK) {
            ACTOR->stateFlags &= ~AYA_DARKNESS_MASK;
        }
        if ((ACTOR->stateFlags & AYA_STIFFNESS_MASK) != AYA_STIFFNESS_MASK) {
            ACTOR->stateFlags &= ~AYA_STIFFNESS_MASK;
        }
        dmg = 0x02580000;
        ACTOR->stateFlags &= ~0x1000;
        break;
    case PE_SPELL_BARRIER:
        {
            Combatant *p = ACTOR;
            p->stateFlags |= AYA_BARRIER;
            p->exp_or_acc -= p->maxAtk / 3;
        }
        break;
    case PE_SPELL_SCAN:
        {
            /* Scan: note which of the target's status pairs read 2. */
            Combatant *e = ent->core;
            g_BattleResultFlagsRecord.bits.scanPair3 = (e->statusFlags2 & 3) == 2;
            g_BattleResultFlagsRecord.bits.scanPairC = (e->statusFlags2 & 0xC) == 8;
            g_BattleResultFlagsRecord.bits.scanPair30 = (e->statusFlags2 & 0x30) == 0x20;
            g_BattleResultFlagsRecord.bits.scanPairC000 = (e->statusFlags2 & 0xC000) == 0x8000;
            g_BattleResultFlagsRecord.bits.scanPair30000 = (e->statusFlags2 & 0x30000) == 0x20000;
            g_BattleResultFlagsRecord.bits.scanPair3000 = (e->statusFlags2 & 0x3000) == 0x2000;
            g_BattleResultFlagsRecord.bits.scanPairC00 = (e->statusFlags2 & 0xC00) == 0x800;
            if (((*(u32 *)&((Combatant *)ent->core)->statusFlags2 >> 6) & 3) == 2) {
                g_BattleResultFlagsRecord.bits.scanPairC0 = 1;
            } else {
                g_BattleResultFlagsRecord.bits.scanPairC0 = 0;
            }
            g_BattleResultFlagsRecord.bits.messageId = 0x4B;
            g_BattleActiveEntity = ent;
            g_BattleResultFlagsRecord.bits.infoPanel = 1;
            dmg = 0x320000;
        }
        break;
    case PE_SPELL_SLOW:
        /* Slow: certain when the target's pair reads 2, a coin flip at 0. */
        {
            s32 pair = (*(u32 *)&((Combatant *)ent->core)->statusFlags2 >> 8) & 3;
            if (pair != 0 ? pair == 2 : !(rand() & 1)) {
                Combatant *e = ent->core;
                e->coreFlags |= 1;
            }
            dmg = 0x5A0000;
        }
        break;
    case PE_SPELL_HASTE:
        /* Haste: also cures a stiffness affliction. */
        {
            Combatant *p = ACTOR;
            COMBATANT_STATE_BITS(p).haste = 1;
            g_ActorEffectFlag100Timer = 0x1C2;
            if (COMBATANT_STATE_BITS(p).stiffness == AYA_STIFFNESS_SLOW_VALUE ||
                COMBATANT_STATE_BITS(p).stiffness == AYA_STIFFNESS_LOCK_VALUE) {
                COMBATANT_STATE_BITS(p).stiffness = 0;
            }
            dmg = 0xC80000;
        }
        break;
    case PE_SPELL_CONFUSE:
        /* Confuse: certain when the target's pair reads 2, a coin flip at 0. */
        {
            s32 pair = (*(u32 *)&((Combatant *)ent->core)->statusFlags2 >> 0xA) & 3;
            if (pair != 0 ? pair == 2 : !(rand() & 1)) {
                s32 r = rand();
                Combatant *e = ent->core;
                s32 flags = e->coreFlags & ~0xE;
                flags |= (((r % 3) + 3) & 7) * 2;
                e->coreFlags = flags;
                Entity_SetActionMode(ent, 4);
                ent->entityFlags |= 0x1000;
            }
            dmg = 0x960000;
        }
        break;
    case PE_SPELL_GENE_HEAL:
        dmg = 0x01900000;
        ACTOR->stateFlags |= AYA_GENE_HEAL;
        break;
    case PE_SPELL_PRERAISE:
        dmg = 0x03E80000;
        ACTOR->stateFlags |= AYA_PRERAISE;
        break;
    case PE_SPELL_FULL_RECOVER:
        /* Full recovery: HP to maximum and every ailment cured. */
        {
            Combatant *actor = ACTOR;
            s32 flags = actor->stateFlags;
            actor->curHP = actor->maxHP;
            if ((flags & AYA_POISON_MASK) != AYA_POISON_MASK) {
                actor->stateFlags = flags & ~AYA_POISON_MASK;
            }
        }
        if ((ACTOR->stateFlags & AYA_CONFUSION_MASK) != AYA_CONFUSION_MASK) {
            ACTOR->stateFlags &= ~AYA_CONFUSION_MASK;
            g_FieldMoveLockRecord.flags &= ~FIELD_MOVE_CONFUSED;
        }
        if ((ACTOR->stateFlags & AYA_DARKNESS_MASK) != AYA_DARKNESS_MASK) {
            ACTOR->stateFlags &= ~AYA_DARKNESS_MASK;
        }
        if ((ACTOR->stateFlags & AYA_STIFFNESS_MASK) != AYA_STIFFNESS_MASK) {
            ACTOR->stateFlags &= ~AYA_STIFFNESS_MASK;
        }
        dmg = 0x05140000;
        ACTOR->stateFlags &= ~0x1000;
        break;
    case PE_SPELL_LIBERATE:
        ACTOR->stateFlags = (ACTOR->stateFlags | 0x80000) & ~0x200000;
        break;
    }
    if ((s16)ACTOR->maxHP < (s16)ACTOR->curHP) {
        ACTOR->curHP = ACTOR->maxHP;
    }
    /* Armor attribute 0x200 cuts the PE cost by a third. */
    if (!(ACTOR->attributes->effectFlags & 0x200)) {
        ACTOR->exp_or_acc -= dmg;
        return;
    }
    ACTOR->exp_or_acc -= (dmg * 2) / 3;
}

/* Advances Aya's 17-state action, saving her pose, visiting the selected
 * targets, and restoring normal animation/visibility at completion.
 * The saved pose is written through the position and rotation records:
 * their field stores may alias the player's fields, which keeps retail's
 * load/store interleaving without pins.
 * Case 9 writes the evasion outcome record and the target's core flag bits
 * as fields, so the reaction byte is read back after the flag stores.
 * Matching debt: register pins and empty compiler barriers retain
 * retail allocation and scheduling in the remaining cases. */
s32 Battle_StepAyaAction(void)
{
  BattleEntity *targetEntity;
  BattleEntity *effectTarget;
  BattleEntity *entity;
  s16 targetAngle;
  BattleEntity *assetOwner;
  s32 targetDistance;
  s32 slot;
  s32 flags;
  register u32 scratch asm("$3");
  s32 finished;
  s8 attackStep;
  u32 effectId;
  u32 targetEffectId;
  int hitCountMask;
  register s32 transfer asm("$2");
  EnemyCombatant *targetCore;
  finished = 0;
  switch (g_AyaActionStep)
  {
    case 0:
    {
      BattleEntity *player;
      Scene_InitEntityPlayer(1);
      player = D_8009CD10;
      g_AyaSavedPos.x = player->posX.fixed;
      g_AyaSavedPos.y = player->posY.fixed;
      g_AyaSavedPos.z = player->posZ.fixed;
      g_AyaSavedRotation.x = player->rotationX;
      g_AyaAttackStep = 0;
      g_AyaSavedRotation.facing = player->facingAngle;
      g_AyaSavedRotation.z = player->rotationZ;
      D_8009CDA8 &= ~4;
      player->entityFlags |= 0x80;
      Anim_SetInterpRate(&player->renderObject, 30);
      D_8009CD10->renderObject.flags_9C |= 2;
      Anim_SetInterpRate(&g_PlayerBattleEntity, 0x1E);
      g_PlayerBattleEntity.flags_9C |= 2;
      Pm_StopLowerHalf();
      g_BattleEffectPmSlot = Scene_LoadRoomAssets(0x6BU, D_8009CD10);
      g_AyaActionStep += 1;
      break;
    }

    case 1:
      Scene_InitEntityPlayer(1);
      if (D_8009CD10->renderObject.variant_visible == 0)
    {
      if (g_PlayerBattleEntity.variant_visible == 0)
      {
        g_AyaActionStep++;
        break;
      }
    }
      break;

    case 2:
      if (Scene_InitEntityPlayer(1) == 0)
    {
      Entity_SetActionMode(D_8009CD10, 5);
      {
        BattleEntity *player = D_8009CD10;
        player->renderObject.variant_visible = 1;
        player->entityFlags |= 0x100;
        Anim_SetInterpRate(&D_8009CD10->renderObject, 0x1E);
      }
      D_8009CD10->renderObject.flags_9C |= 4;
      assetOwner = D_8009CD10;
      Scene_LoadRoomAssets(0x6CU, assetOwner);
      g_AyaActionTimer = 0x1E;
      g_AyaActionStep++;
      break;
    }
      break;

    case 3:
      if (g_AyaActionTimer == 0)
    {
      g_AyaActionStep += 1;
      D_8009CD10->entityFlags &= ~0x100;
    }
    else
    {
      g_AyaActionTimer -= 1;
    }

    /* State 3 also waits for the animation handled by state 4. */
    case 4:
      if (D_8009CD10->animLastFrame == (u16)(D_8009CD10->animFrame >> 16))
    {
      entity = (BattleEntity *)g_FieldActorListHead;
      if (entity != 0)
      {
        do
        {
          if ((entity != D_8009CD10) && (entity->core != 0))
          {
            entity->motionX = 0;
            entity->motionY = 0;
            entity->motionZ = 0;
          }
          entity = entity->next;
        }
        while (entity != 0);
      }
      Pm_Stop(g_BattleEffectPmSlot, D_8009CD10, 0);
      effectId = 0x6D;
      Scene_LoadRoomAssets(effectId, D_8009CD10);
      g_AyaActionStep += 1;
      break;
    }

    default:
      break;

    case 5:
      Entity_SetActionMode(D_8009CD10, 6);
      Anim_SetInterpRate(&D_8009CD10->renderObject, 0xF);
      g_AyaActionStep += 1;
      D_8009CD10->renderObject.flags_9C |= 2;
      break;

    case 6:
      if (D_8009CD10->renderObject.variant_visible == 0)
    {
      {
        s32 product;
        int reachScale;
        targetEntity = g_BattleActionQueue[g_BattleActionIndex].actor;
        reachScale = (((EnemyCombatant *) targetEntity->core)->statusFlags2 >> 0x13) & 0x1F;
        targetDistance = ((targetEntity->renderObject.hit_cylinder.radius * reachScale) / 10) + D_8009CD10->renderObject.hit_cylinder.radius;
        targetAngle = Battle_CalcAngleToTarget(&targetEntity->renderObject, &g_AyaSavedPos);
        D_8009CD10->facingAngle = targetAngle;
        product = targetDistance * rsin(targetAngle);
        asm("" : "=r"(product) : "0"(product));
        D_8009CD10->posX.fixed = (g_BattleActionQueue[g_BattleActionIndex].actor->renderObject.target_x << 16) + (product * 16);
        product = targetDistance * rcos(D_8009CD10->facingAngle);
        D_8009CD10->posZ.fixed = (g_BattleActionQueue[g_BattleActionIndex].actor->renderObject.target_z << 16) + (product * 16);
      }
      D_8009CD10->baseX = D_8009CD10->posX.fixed;
      D_8009CD10->baseY = D_8009CD10->posY.fixed;
      D_8009CD10->baseZ = D_8009CD10->posZ.fixed;
      Asset_Find08Alt(0x4B6, 0, D_8009CD10->posX.parts.integer, D_8009CD10->posY.parts.integer, D_8009CD10->posZ.parts.integer);
      g_AyaActionTimer = 0x1E;
      g_AyaActionStep++;
      break;
    }
      break;

    case 7:
      if (D_8009CD10->animLastFrame == D_8009CD10->animPrev.parts.integer)
    {
      Entity_SetActionMode(D_8009CD10, 7);
      D_8009CD10->animFrame = D_8009CD10->animLastFrame << 0xF;
    }
      if (g_AyaActionTimer == 0)
    {
      D_8009CD10->renderObject.variant_visible = 1;
      Anim_SetInterpRate(&D_8009CD10->renderObject, 0xF);
      g_AyaActionStep += 1;
      D_8009CD10->renderObject.flags_9C |= 4;
    }
    else
    {
      g_AyaActionTimer -= 1;
    }
      break;

    case 8:
      if (D_8009CD10->animLastFrame == D_8009CD10->animPrev.parts.integer)
    {
      {
        u16 flags = D_8009CD10->renderObject.flags_9C;
        flags |= 0x20;
        D_8009CD10->renderObject.flags_9C = flags;
      }
      Entity_SetActionMode(D_8009CD10, ((g_AyaAttackStep * 2) + 8) & 0xFFFE);
      g_AyaActionStep += 1;
      break;
    }
      break;

    case 9:
      if (D_8009CD10->animLastFrame == D_8009CD10->animPrev.parts.integer)
    {
      Entity_SetActionMode(D_8009CD10, ((g_AyaAttackStep * 2) + 9) & 0xFFFF);
      {
        unsigned targetIndex;
        EnemyCoreFlagBits *bits;
        targetIndex = g_BattleActionIndex;
        g_EvasionOutcome.reaction = 2;
        g_EvasionOutcome.status = 1;
        targetCore = g_BattleActionQueue[targetIndex].actor->core;
        bits = (EnemyCoreFlagBits *)&targetCore->coreFlags;
        bits->hitState = 1;
        bits->category = g_ActiveActor->action->attackWord >> 20;
        bits->reaction = g_EvasionOutcome.reaction;
        if (g_AyaAttackStep == 2)
        {
          effectTarget = g_BattleActionQueue[targetIndex].actor;
          targetEffectId = 0x6F;
        }
        else
          if (g_AyaAttackStep == 5)
        {
          effectTarget = g_BattleActionQueue[targetIndex].actor;
          targetEffectId = 0x71;
        }
        else
        {
          targetEffectId = 0x6E;
          if (g_AyaAttackStep == 6)
          {
            effectTarget = g_BattleActionQueue[targetIndex].actor;
            targetEffectId = 0x70;
          }
          else
          {
            effectTarget = g_BattleActionQueue[targetIndex].actor;
          }
        }
        Scene_LoadRoomAssets(targetEffectId, effectTarget);
      }
      attackStep = ((u8) g_AyaAttackStep) + 1;
      g_AyaAttackStep = attackStep;
      if (attackStep < 7)
      {
        slot = g_BattleActionIndex & 0xFF;
        if (g_BattleActionQueue[slot].actor == g_BattleActionQueue[slot + 1].actor)
        {
          {
            u8 next = g_BattleActionIndex + 1;
            g_AyaActionStep = 8;
            g_BattleActionIndex = next;
          }
          break;
        }
      }
      g_AyaActionStep += 1;
      break;
    }
      break;

    case 10:
      if (D_8009CD10->animLastFrame == D_8009CD10->animPrev.parts.integer)
    {
      Entity_SetActionMode(D_8009CD10, 7);
      if (g_AyaAttackStep < 7)
      {
        g_AyaActionStep = 5;
        g_BattleActionIndex += 1;
        break;
      }
      Anim_SetInterpRate(&D_8009CD10->renderObject, 15);
      g_AyaActionStep += 1;
      D_8009CD10->renderObject.flags_9C |= 2;
      break;
    }
      break;

    case 11:
    {
      BattleEntity *player = D_8009CD10;
      s32 *saved;
      s32 x;
      s32 y;
      register int effect;
      if (player->renderObject.variant_visible == 0)
      {
        effect = 0x4B6;
        saved = &g_AyaSavedPos.x;
        transfer = *saved;
        player->posX.fixed = transfer;
        x = player->posX.parts.integer;
        transfer = g_AyaSavedPos.y;
        player->posY.fixed = transfer;
        y = player->posY.parts.integer;
        transfer = g_AyaSavedPos.z;
        player->posZ.fixed = transfer;
        transfer = *saved;
        player->baseX = transfer;
        transfer = g_AyaSavedPos.y;
        player->baseY = transfer;
        transfer = g_AyaSavedPos.z;
        player->baseZ = transfer;
        transfer = g_AyaSavedRotation.x;
        player->rotationX = transfer;
        transfer = g_AyaSavedRotation.facing;
        player->facingAngle = transfer;
        scratch = g_AyaSavedRotation.z;
        player->rotationZ = scratch;
        transfer = player->posZ.parts.integer;
        Asset_Find08Alt(effect, 0, x, y, transfer);
        g_AyaActionTimer = 0x1E;
        g_AyaActionStep++;
        break;
      }
      break;
    }

    case 12:
      if (g_AyaActionTimer == 0)
    {
      Entity_SetActionMode(D_8009CD10, 7);
      D_8009CD10->renderObject.variant_visible = 1;
      Anim_SetInterpRate(&D_8009CD10->renderObject, 0x1E);
      g_BattleCrossFlashStop = 1;
      g_AyaActionTimer = 0xF;
      g_AyaActionStep += 1;
      D_8009CD10->renderObject.flags_9C |= 4;
      break;
    }
      g_AyaActionTimer -= 1;
      break;
      break;

    case 13:
      if (g_AyaActionTimer == 0)
    {
      if (D_8009CD10->animLastFrame == D_8009CD10->animPrev.parts.integer)
      {
        Entity_SetActionMode(D_8009CD10, 4);
        g_AyaActionStep += 1;
        break;
      }
      break;
    }
      g_AyaActionTimer -= 1;
      break;

    case 14:
      if (D_8009CD10->animLastFrame == (u16)(D_8009CD10->animFrame >> 16))
    {
      D_8009CD10->entityFlags |= 0x100;
      Anim_SetInterpRate(&D_8009CD10->renderObject, 0xF);
      effectId = 0x72;
      D_8009CD10->renderObject.flags_9C |= 2;
      Scene_LoadRoomAssets(effectId, D_8009CD10);
      g_AyaActionStep += 1;
      break;
    }
      break;

    case 15:
      if (D_8009CD10->renderObject.variant_visible == 0)
    {
      if (Scene_InitEntityPlayer(0) == 0)
      {
        Entity_SetActionMode(D_8009CD10, g_ActiveActor->actionMode12);
        {
          register int rate asm("$5") = 30;
          BattleEntity *player;
          register RenderObjectEntity *render asm("$4");
          unsigned flags;
          scratch = ~0x100;
          asm("" : "=r"(scratch) : "0"(scratch), "r"(rate));
          player = D_8009CD10;
          D_8009CDA8 |= 4;
          player->renderObject.variant_visible = 1;
          render = &D_8009CD10->renderObject;
          flags = player->entityFlags;
          flags &= scratch;
          scratch = ~0x80;
          flags &= scratch;
          player->entityFlags = flags;
          Anim_SetInterpRate(render, rate);
        }
        D_8009CD10->renderObject.flags_9C |= 4;
        g_PlayerBattleEntity.variant_visible = 1;
        Anim_SetInterpRate(&g_PlayerBattleEntity, 0x1E);
        {
          g_AyaActionTimer = 0xF;
          g_PlayerBattleEntity.flags_9C |= 4;
          g_AyaActionStep++;
        }
        break;
      }
    }
      break;

    case 16:
      if (g_AyaActionTimer == 0)
    {
      {
        int mask = ~0x100;
        unsigned flags = g_GameStateFlags;
        u8 next = g_BattleActionIndex + 1;
        g_GameStateFlags = flags & mask;
        g_BattleActionIndex = next;
      }
      finished = 1;
    }
    else
    {
      g_AyaActionTimer -= 1;
      break;
    }
      break;

  }

  return finished;
}
