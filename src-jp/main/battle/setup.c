/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/battle.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_reset_stats.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_setup.h"
#include "pe1/battle_status.h"
#include "pe1/game_state.h"
#include "pe1/gte_types.h"
#include "pe1/inventory.h"
#include "pe1/menu_flags.h"
#include "pe1/player_entity.h"
#include "pe1/pm.h"
#include "pe1/random.h"
#include "pe1/textbox.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
s8 g_BattleHaltTimer;
u16 g_BattleHitActionPending;
u8 g_BattleActionAnimModes[4];
s8 g_BattleTargetCycleMode;
u8 g_BattleRemainingActions;
int g_BattleAttackPmSlot;
s32 g_BattleAilmentPmSlot;
s32 g_BattleEffectPmSlot;
u8 g_BattleHitPending;
unsigned char g_BattleActionQueueCount;
s8 g_BattleTargetIndex;

void Battle_BeginPlayerAction(void) {
    BattleEntity *player;
    Combatant *actor;
    int mask;
    int flags;
    u32 *entityFlags;
    s32 *stateFlags;

    mask = -0x101;
    D_8009CDA8 |= 1;
    player = D_8009CD10;
    entityFlags = &player->entityFlags;
    flags = *entityFlags;
    actor = g_ActiveActor;
    flags &= mask;
    *entityFlags = flags;
    stateFlags = &actor->stateFlags;
    *stateFlags |= 0x10000;
    g_BattleHitActionPending = 0;
    Entity_SetActionMode(player, 0x12);
    Battle_FlushScriptSounds();
}

void Battle_ReturnToIdle(void) {
    Combatant *state;
    BattleEntity *actor;
    int mask_state;
    int flags;
    s32 *stateFlags;

    mask_state = 0xFFFEFFFF;
    D_8009CDA8 &= -2;
    state = g_ActiveActor;
    actor = (BattleEntity *)g_PlayerEntity;
    stateFlags = &state->stateFlags;
    flags = *stateFlags;
    flags &= mask_state;
    *stateFlags = flags;
    actor->motionX = 0;
    actor->motionY = 0;
    actor->motionZ = 0;
    Entity_SetActionMode(actor, state->actionMode12);
}

void Battle_HaltOnPositiveX(void) {
    Combatant *state;
    BattleEntity *actor;
    Combatant *state2;
    BattleEntity *actor2;
    int flags;

    state = g_ActiveActor;
    if ((s16)state->curHP > 0) {
        actor = (BattleEntity *)g_PlayerEntity;
        actor->motionX = 0;
        actor->motionY = 0;
        actor->motionZ = 0;
        Entity_SetActionMode(actor, 0x12);

        state2 = g_ActiveActor;
        actor2 = (BattleEntity *)g_PlayerEntity;
        flags = state2->stateFlags;
        g_BattleHaltTimer = 0;
        flags |= 0x2000;
        state2->stateFlags = flags;
        actor2->entityFlags &= ~0x100;
        Battle_FlushScriptSounds();
    }
}

void Battle_InitEntityColors(void) {
    s32 flags;
    s32 *stateFlags;

    Entity_SetActionMode(D_8009CD10, g_ActiveActor->actionMode12);
    stateFlags = &g_ActiveActor->stateFlags;
    flags = *stateFlags;
    g_BattleHaltTimer = 0;
    *stateFlags = flags & ~0x2000;
    g_BattleHitActionPending = 0;
    g_BattlePeGauges[0][1].r0 = 0xFF;
    g_BattlePeGauges[0][1].g0 = 0x3D;
    g_BattlePeGauges[0][1].b0 = 0x81;
    g_BattlePeGauges[0][1].r1 = 0x83;
    g_BattlePeGauges[0][1].g1 = 0x13;
    g_BattlePeGauges[0][1].b1 = 1;
    g_BattlePeGauges[0][1].r2 = 0xFF;
    g_BattlePeGauges[0][1].g2 = 0x3D;
    g_BattlePeGauges[0][1].b2 = 0x81;
    g_BattlePeGauges[0][1].r3 = 0x83;
    g_BattlePeGauges[0][1].g3 = 0x13;
    g_BattlePeGauges[0][1].b3 = 1;
    g_BattlePeGauges[1][1].r0 = 0xFF;
    g_BattlePeGauges[1][1].g0 = 0x3D;
    g_BattlePeGauges[1][1].b0 = 0x81;
    g_BattlePeGauges[1][1].r1 = 0x83;
    g_BattlePeGauges[1][1].g1 = 0x13;
    g_BattlePeGauges[1][1].b1 = 1;
    g_BattlePeGauges[1][1].r2 = 0xFF;
    g_BattlePeGauges[1][1].g2 = 0x3D;
    g_BattlePeGauges[1][1].b2 = 0x81;
    g_BattlePeGauges[1][1].r3 = 0x83;
    g_BattlePeGauges[1][1].g3 = 0x13;
    g_BattlePeGauges[1][1].b3 = 1;
}

unsigned char g_BattleActionQueueCount;
unsigned char g_BattleActionIndex;
unsigned char g_BattleRemainingActions;
signed char g_BattleActiveTurnSlot;
signed char g_BattleItemMenuState;

void Save_ResetGlobalFlags(void) {
    g_BattleActionQueueCount = 0;
    g_BattleActionIndex = 0;
    g_BattleRemainingActions = 0;
    g_BattleActiveTurnSlot = 0;
    g_BattleItemMenuState = 0;
}

void Battle_Init(void)
{
    u8 index;
    BattleInitSlot *records;
    BattleAction **action;
    BattleAction *config0;
    BattleAction *config1;
    BattleAction *config2;
    BattleAction *config3;
    BattleAction *config4;
    BattleAction *config5;

    Pm_StopAll();
    index = 0;
    records = g_BattleActionQueue;
    g_BattleAttackPmSlot = -1;
    g_BattleImpactPmSlot = -1;
    g_BattleEffectPmSlot = -1;
    g_BattleAilmentPmSlot = -1;

    do {
        g_BattleActionQueue[index].actor = 0;
        records[index].actionGeneration = 0;
        g_BattleActionQueue[index].command = 0;
        index++;
    } while (index < 45);

    action = &g_ActiveActor->action;
    config0 = *action;
    g_BattleTargetIndex = 0;
    g_BattleTargetCycleMode = 0;
    g_BattleHitPending = 0;
    config1 = *action;
    g_BattleActiveTurnSlot = (config0->turnWord >> 4) & 3;
    config2 = *action;
    g_BattleRemainingActions = config1->turnWord & 0xF;
    g_BattleActionAnimModes[0] = config2->animMode[0];
    config3 = *action;
    g_BattleActionAnimModes[1] = config3->animMode[1];
    config4 = *action;
    g_BattleActionAnimModes[2] = config4->animMode[2];
    config5 = *action;
    g_BattleActionAnimModes[3] = config5->animMode[3];

    Draw_ResetTextboxes();
    g_BattleSaveOverlayActive = 0;
    g_BattleResultFlags &= ~0x300;
    Battle_InitFadeVars();
}

unsigned char g_BattleActionQueueCount;

signed char Pad_GetMenuPressedBitOrDisabled(void) {
    if (g_ActiveActor->stateFlags & 0x10000) {
        return -1;
    }
    return (signed char)g_BattleActionQueueCount;
}

int Battle_IsActive(void) {
    if (g_BattleActionQueueCount != 0) {
        return 0;
    }
    if ((g_GameStateFlags & 2) == 0 || (g_ActiveActor->stateFlags & 0x10000) == 0) {
        return 1;
    }
    return 0;
}

int Pad_IsMenuConfirmAvailable(void) {
    if (g_BattleActionQueueCount != 0) {
        return 0;
    }
    if ((g_GameStateFlags & 2) == 0 || (g_ActiveActor->stateFlags & 0x10000) == 0) {
        return 1;
    }
    return 0;
}

/* MASPSX_FORCE_G0 keeps the battle pointers and the effect result absolute,
 * as retail; the COMMON asset result supplies the small-data metadata of the
 * one $gp-relative word. */
int g_BattleAttackPmSlot;

void Battle_DispatchEntityEffect(void) {
    int action_id;
    BattleAction *action;
    BattleAction *action2;
    int action_id2;
    BattleEntity *next_entity;
    int next_id;
    int result;
    u32 word10;

    action = g_ActiveActor->action;
    action_id = action->actionCode.actionId;

    if (action_id == 6) {
        if ((action->attackWord & 0x3FF) == 0) {
            BattleCmd_CommitAmmoAndUpdate(action);
        }
        g_BattleAttackPmSlot = Scene_LoadRoomAssets(3, D_8009CD10);
        return;
    }

    if (action_id == 8) {
        g_BattleImpactPmSlot = Scene_LoadRoomAssets(6, D_8009CD10);
        return;
    }

    if ((action->attackWord & 0x3FF) == 0) {
        BattleCmd_CommitAmmoAndUpdate(action);
    }

    action2 = g_ActiveActor->action;
    action_id2 = action2->actionCode.actionId;

    if (action_id2 == 5 || ((word10 = action2->turnWord) & 0x1F00) != 0) {
        result = Scene_LoadRoomAssets(0, D_8009CD10);
        next_entity = D_8009CD10;
        next_id = 5;
    } else {
        if ((action_id2 == 2) || ((word10 & 0xC0) == 0x80)) {
            result = Scene_LoadRoomAssets(1, D_8009CD10);
        } else {
            result = Scene_LoadRoomAssets(2, D_8009CD10);
        }
        next_entity = D_8009CD10;
        next_id = 4;
    }

    g_BattleAttackPmSlot = result;
    g_BattleImpactPmSlot = Scene_LoadRoomAssets(next_id, next_entity);
}

void Battle_CheckEvasion(BattleEntity *actor, BattleEvasionOutcome *outcome,
                         int attackType)
{
    BattleAction *action;
    int range;
    int distance;
    int roll;
    int threshold;
    int special;
    int hit;

    outcome->reaction = 0;
    if (!(actor->entityFlags & 0x4000)) {
    if (g_ActiveActor->stateFlags & 0x100000) {
        outcome->status = 1;
        outcome->reaction = 2;
        return;
    }

    outcome->status = 0;
    range = AYA_DARKNESS_RANGE;
    if ((g_ActiveActor->stateFlags & AYA_DARKNESS_MASK) != AYA_DARKNESS) {
        range = g_ActiveActor->action->range;
    }
    distance = Battle_CalcDistToPlayer(actor, D_8009CD10) * 1000 / range;
    {
        int random = rand();
        int quotient = random / 100;
        roll = random - quotient * 100;
    }

    action = g_ActiveActor->action;
    if (action->actionCode.actionId == 8) {
        if (distance >= 1001) return;
        threshold = ((action->turnWord >> 17) & 1) * 20;
        if (distance >= 801) threshold += 5;
        else if (distance >= 501) threshold += 20;
        else threshold += 50;
        if ((s16)roll < threshold) outcome->reaction = 3;
        outcome->status = 1;
        return;
    }

    if (action->actionCode.actionId == 6 && attackType != 0) {
        outcome->status = 1;
        switch (attackType) {
        case 2:
            outcome->reaction = 1;
            break;
        case 3:
            outcome->reaction = 2;
            break;
        default:
            outcome->reaction = 4;
            break;
        }
        return;
    }

    if (distance >= 1801) return;
    if (distance >= 1501) {
    hit = (s16)roll < 25;
    if (!hit) return;
    outcome->status = 1;
    outcome->reaction = 1;
    return;
    }
below_1501:
    if (distance >= 1201) {
    hit = (s16)roll < 50;
    if (!hit) return;
    outcome->status = 1;
    outcome->reaction = 1;
    return;
    }
below_1201:
    if (distance >= 1001) {
    hit = (s16)roll < 80;
    if (!hit) return;
    outcome->status = 1;
    outcome->reaction = 1;
    return;

    }
close_range:
    special = (g_ActiveActor->action->turnWord >> 17) & 1;
    threshold = special * 20;
    if (distance >= 801) threshold += 5;
    else if (distance >= 501) threshold += 20;
    else threshold += 50;
    outcome->reaction = (s16)roll < threshold ? 3 : 2;
    outcome->status = 1;
    return;

    }
invalid:
    outcome->status = -1;
done:
    return;
}

#define NULL ((void *)0)

void Battle_BuildTargetList(void) {
    s32 entityFlags;
    s32 nextIndex;
    register s32 finalCount asm("$2");
    s32 pad_[2];
    s32 distance;
    s16 angle;
    EnemyCombatant *candidateCore;
    BattleEntity *player;
    BattleEntity *candidate;

    candidate = (BattleEntity *)g_FieldActorListHead;
    g_BattleTargetIndex = 0;
    if (candidate != NULL) {
        do {
            player = D_8009CD10;
            if (candidate != player) {
                candidateCore = candidate->core;
                if (candidateCore != NULL) {
                    entityFlags = candidate->entityFlags;
                    if (((entityFlags & 0x2040) != 0x40) && !(entityFlags & 0x4000) &&
                        (candidateCore->hpAlive > 0)) {
                        g_BattleTargetList[g_BattleTargetIndex].actor = candidate;
                        distance = Battle_CalcDistToPlayer(candidate, player);
                        g_BattleTargetList[g_BattleTargetIndex].dist = distance;
                        angle = ratan2(
                            candidate->renderObject.target_x -
                                D_8009CD10->posX.parts.integer,
                            candidate->renderObject.target_z -
                                D_8009CD10->posZ.parts.integer);
                        nextIndex = g_BattleTargetIndex + 1;
                        g_BattleTargetList[g_BattleTargetIndex].angle = angle;
                        angle = nextIndex;
                        g_BattleTargetIndex = angle;
                    }
                }
            }
            candidate = candidate->next;
        } while (candidate != NULL);
    }
    if (g_BattleTargetIndex >= 2) {
        Battle_SortTargets((BattleTargetWords *)g_BattleTargetList, 0, (s8) (g_BattleTargetIndex - 1));
    }
    finalCount = g_BattleTargetIndex;
    __asm__("" : "=r"(finalCount) : "0"(finalCount));
    g_BattleTargetList[finalCount].actor = NULL;
}

void Battle_SortTargets(BattleTargetWords *records, signed char first, signed char last)
{
    signed char pivot;
    signed char scan;

    if (first < last) {
        Battle_SwapRecords(records, first, (signed char)((first + last) / 2));
        pivot = first;
        for (scan = pivot; scan <= last; scan++) {
            if (g_BattleTargetList[scan].dist < g_BattleTargetList[first].dist) {
                pivot++;
                Battle_SwapRecords(records, pivot, scan);
            }
        }
        Battle_SwapRecords(records, first, pivot);
        Battle_SortTargets(records, first, (signed char)(pivot - 1));
        Battle_SortTargets(records, (signed char)(pivot + 1), last);
    }
}

void Battle_SwapRecords(BattleTargetWords *records, int from_arg, int to_arg) {
    BattleTargetWords tmp;
    int from_index;
    int to_index;

    from_index = (signed char)from_arg;
    to_index = (signed char)to_arg;

    tmp = records[from_index];
    records[from_index] = records[to_index];
    records[to_index] = tmp;
}

/* Copy the action the battle menu chose into the player's action record. */
void Battle_CopyPadStateToRecord(void) {
    BattleAction *record = g_ActiveActor->action;
    BattleAttackBits *attack = (BattleAttackBits *)&record->attackWord;
    BattleTurnBits *turn = (BattleTurnBits *)&record->turnWord;

    record->powerBonus = g_BattleMenuActionRecord.powerBonus;
    record->range = g_BattleMenuActionRecord.range;
    record->field04 = g_BattleMenuActionRecord.field04;
    record->actionCode.actionId = g_BattleMenuActionRecord.actionCode.actionId;
    record->hitTrigger = g_BattleMenuActionRecord.hitTrigger;
    attack->count = g_BattleMenuAttackBits.count;
    attack->power = g_BattleMenuAttackBits.power;
    attack->category = g_BattleMenuAttackBits.category;
    turn->actions = g_BattleMenuTurnBits.actions;
    turn->slot = g_BattleMenuTurnBits.slot;
    turn->mode = g_BattleMenuTurnBits.mode;
    turn->element8 = g_BattleMenuTurnBits.element8;
    turn->element9 = g_BattleMenuTurnBits.element9;
    turn->element10 = g_BattleMenuTurnBits.element10;
    turn->element11 = g_BattleMenuTurnBits.element11;
    turn->element12 = g_BattleMenuTurnBits.element12;
    turn->hitState = g_BattleMenuTurnBits.hitState;
    turn->flag15 = g_BattleMenuTurnBits.flag15;
    turn->flag16 = g_BattleMenuTurnBits.flag16;
    turn->flag17 = g_BattleMenuTurnBits.flag17;
}

/* Copy the enemy attributes the menu record carries into the active actor. */
void Battle_SyncEnemyAttributes(void) {
    BattleAttributes *source = (BattleAttributes *)&g_BattleMenuActionRecord;
    BattleAttributes *attributes = g_ActiveActor->attributes;
    BattleEffectBits *effects = (BattleEffectBits *)&attributes->effectFlags;

    attributes->parameterWord.fields.first = source->parameterWord.fields.first;
    attributes->parameterWord.fields.second = source->parameterWord.fields.second;
    attributes->parameterWord.fields.third = source->parameterWord.fields.third;
    attributes->parameterWord.fields.reserved = source->parameterWord.fields.reserved;
    effects->resistPoison = g_BattleMenuEffectBits.resistPoison;
    effects->resistConfusion = g_BattleMenuEffectBits.resistConfusion;
    effects->resistDarkness = g_BattleMenuEffectBits.resistDarkness;
    effects->resistStiffness = g_BattleMenuEffectBits.resistStiffness;
    effects->resistSteal = g_BattleMenuEffectBits.resistSteal;
    effects->armorMode = g_BattleMenuEffectBits.armorMode;
    effects->mod14 = g_BattleMenuEffectBits.mod14;
    effects->mod16 = g_BattleMenuEffectBits.mod16;
    effects->mod11 = g_BattleMenuEffectBits.mod11;
    effects->mod17 = g_BattleMenuEffectBits.mod17;
    effects->mod12 = g_BattleMenuEffectBits.mod12;
    effects->autoMedicine = g_BattleMenuEffectBits.autoMedicine;
    effects->autoCure = g_BattleMenuEffectBits.autoCure;
    effects->mod13 = g_BattleMenuEffectBits.mod13;
    effects->mod15 = g_BattleMenuEffectBits.mod15;
}

/* MASPSX_FORCE_G0 keeps the game-state flags absolute, as retail; the COMMON
 * script sound cursor and count supply the small-data metadata of the two
 * $gp-relative bytes. */
u8 g_BattleActionIndex;
u8 g_BattleActionQueueCount;

void Battle_FlushScriptSounds(void) {
    unsigned int index;
    int flags;
    int mask;
    int item;

    index = g_BattleActionIndex;
    while ((index & 0xFF) < g_BattleActionQueueCount) {
        item = (u16)g_BattleActionQueue[index & 0xFF].command;
        if ((unsigned int)(item - 3) < 0x180) {
            Inv_AddItem((short)item - 3);
        }

        index++;
    }

    flags = g_GameStateFlags;
    mask = -0x101;
    g_BattleActionQueueCount = 0;
    g_BattleActionIndex = 0;
    g_GameStateFlags = flags & mask;
}

void Battle_StepScriptEntry(void)
{
    BattleInitSlot *entry;
    BattleEntity *entity;
    int command;

    if (g_BattleActionIndex < g_BattleActionQueueCount) {
        entry = &g_BattleActionQueue[g_BattleActionIndex];
        entity = D_8009CD10;
        entity->motionX = 0;
        entity->motionY = 0;
        entity->motionZ = 0;
        if (entity->actionMode >= 4) {
            command = entry->command;
            if (command < 3) {
                Battle_UpdatePlayerTurn();
            } else if (command < 0x183) {
                Battle_ApplyPlayerHit();
            } else if (command < 0x197) {
                Battle_AdvancePhase();
            } else if (command < 0x199) {
                Entity_SetActionMode(entity, 13);
                D_8009CDA8 |= 1;
                g_BattleActionIndex++;
                D_8009CD10->entityFlags |= 0x100;
            } else {
                if ((s8)Battle_RollEscapeChance() == 1) {
                    Entity_SetActionMode(D_8009CD10, g_ActiveActor->actionMode12);
                    g_BattleModeState = 4;
                }
                g_BattleActionIndex++;
            }
        }
    } else {
        g_BattleActionIndex = 0;
        g_BattleActionQueueCount = 0;
    }
}
