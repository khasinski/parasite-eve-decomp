/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/akao.h"
#include "pe1/asset_find.h"
#include "pe1/battle.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_update.h"
#include "pe1/menu_inventory.h"
#include "pe1/player_entity.h"
#include "pe1/random.h"
#include "pe1/render_object.h"
#include "pe1/save.h"
#include "pe1/scene_transition.h"

void Battle_DispatchSpecialAction(int arg0) {
    BattleEntity *actor;

    if ((g_GameStateFlags & 2) == 0) {
        actor = (BattleEntity *)g_PlayerEntity;
        g_ActiveActor = actor->core;
        Asset_Find08Alt(0x453, 1, actor->posX.parts.integer,
                       actor->posY.parts.integer, actor->posZ.parts.integer);
    }

    switch (arg0) {
    case 0x197:
        Battle_CopyPadStateToRecord();
        Battle_SetupEnemyAnims();
        g_ActiveActor->actionMode12 = 0xD;
        break;
    case 0x198:
        Battle_SyncEnemyAttributes();
        break;
    }
}

void Battle_DispatchInit(int arg0) {
    if (g_GameStateFlags & 2) {
        return;
    }

    g_ActiveActor = ((BattleEntity *)g_PlayerEntity)->core;

    switch (arg0) {
    case 0x197:
        Battle_CopyPadStateToRecord();
        Battle_SetupEnemyAnims();
        break;
    case 0x198:
        Battle_SyncEnemyAttributes();
        break;
    default:
        break;
    }
}

int Battle_RollEscapeChance(void) {
    BattleEntity *entity = (BattleEntity *)g_FieldActorListHead;
    s16 highestRank = 0;
    /* Retail narrows the probability after every multiplier. */
    s8 result = 0;
    for (; entity; entity = entity->next) {
        EnemyCombatant *enemy;
        if (entity == D_8009CD10) continue;
        enemy = entity->core;
        if (!enemy || enemy->hpAlive <= 0) continue;
        if (highestRank < (s8)enemy->field04.bytes.rank)
            highestRank = (s8)enemy->field04.bytes.rank;
        if (enemy->statusFlags2 & 0x40000) {
            result = -1;
            break;
        }
    }
    if (!result) {
        int difference = (s8)(g_ActiveActor->field04.bytes.rank - highestRank);
        if (difference >= 2) result = 80;
        else if (difference == 1) result = 40;
        else if (difference == 0) result = 40;
        else if (difference == -1) result = 25;
        else result = 15;
        if ((s16)g_ActiveActor->curHP * 10 < (s16)g_ActiveActor->maxHP)
            result = result * 3 / 2;
        switch (((unsigned int)g_ActiveActor->stateFlags >> 25) & 7) {
        case 1: result = result * 3 / 2; break;
        case 2: result *= 2; break;
        case 3: result *= 3; break;
        case 4: result *= 4; break;
        case 5: result = 100; break;
        }
        {
            int chance = result;
            result = rand() % 100 < chance;
        }
    }
    if (result <= 0) {
        Combatant *player;
        int attempts;
        g_BattleSaveOverlayActive = 1;
        if (result == -1)
            g_CurItemEffectData = g_EscapeBlockedTexts + Save_GetMetadataWindowIndex() * 14;
        else
            g_CurItemEffectData = g_EscapeFailedTexts + Save_GetMetadataWindowIndex() * 15;
        player = g_ActiveActor;
        attempts = ((unsigned int)player->stateFlags >> 25) & 7;
        if (attempts < 5)
            player->stateFlags = (player->stateFlags & 0xF1FFFFFF) |
                (((attempts + 1) & 7) << 25);
    }
    return result;
}

s8 g_BattleTargetIndex;

#define PLAY_TABLE_SOUND(command) \
    { if (g_AkaoBgmHandle[0]) Akao_SendTableCommand(g_AkaoBgmHandle[0], command, 0, 0x80, 0x7F); }
void Battle_CycleTarget(s8 mode) {
    if (g_BattleTargetCount >= 2) {
        if (mode != 1 && mode != 2) {
            if (g_FieldPadPressed & 0x10) {
                Battle_MarkActiveEntities(g_BattleTargetList, g_BattleTargetIndex);
                g_BattleTargetIndex = (g_BattleTargetIndex + 1) % g_BattleTargetCount;
                Battle_InitFadeVars();
                PLAY_TABLE_SOUND(0x44E)
            }
            if (g_FieldPadPressed & 0x40) {
                Battle_MarkActiveEntities(g_BattleTargetList, g_BattleTargetIndex);
                g_BattleTargetIndex = (g_BattleTargetIndex + g_BattleTargetCount - 1) % g_BattleTargetCount;
                Battle_InitFadeVars();
                PLAY_TABLE_SOUND(0x44E)
            }
        }
    } else if (!g_BattleTargetCount) mode = 8;
    switch (mode) {
    case 0: case 1: case 2: case 3: {
        s8 outOfRange;
        BattleEntity *actor;
        Render_AnimationFrame();
        if ((g_ActiveActor->stateFlags & AYA_DARKNESS_MASK) == AYA_DARKNESS)
            outOfRange = g_BattleTargetList[g_BattleTargetIndex].dist > AYA_DARKNESS_RANGE;
        else
            outOfRange = g_ActiveActor->action->range < g_BattleTargetList[g_BattleTargetIndex].dist;
        actor = g_BattleTargetList[g_BattleTargetIndex].actor;
        Battle_DrawStatusOverlay(&actor->renderObject, mode, outOfRange,
            ((EnemyCombatant *)actor->core)->lowAndEntityId.bytes.entityId);
        break;
    }
    case 4: case 5: case 6: case 7: {
        BattleEntity *actor = g_BattleTargetList[g_BattleTargetIndex].actor;
        Battle_DrawStatusOverlay(&actor->renderObject, mode, 0,
            ((EnemyCombatant *)actor->core)->lowAndEntityId.bytes.entityId);
        break;
    }
    case 8: Battle_DrawStatusOverlay(0, 8, 0, 0); break;
    }
    Battle_StepPlayerTurn(g_BattleTargetList, g_BattleTargetIndex, mode);
}

u8 g_BattleHitPending;
u8 g_BattleRemainingActions;
u8 g_BattleActionAnimModes[4];
u8 g_BattleActionQueueCount;

void Battle_SetupEntityTarget(BattleEntity *actor) {
    BattleEntity *target;
    g_BattleHitPending = 0;
    g_BattleRemainingActions = g_ActiveActor->action->turnWord & 15;
    g_BattleActionAnimModes[0] = g_ActiveActor->action->animMode[0];
    g_BattleActionAnimModes[1] = g_ActiveActor->action->animMode[1];
    g_BattleActionAnimModes[2] = g_ActiveActor->action->animMode[2];
    g_BattleActionAnimModes[3] = g_ActiveActor->action->animMode[3];
    target = 0;
    if ((actor->entityFlags & 0x40000000) && !((EnemyCombatant *)actor->core)->deathPersist) {
        for (target = (BattleEntity *)g_FieldActorListHead; target; target = target->next) {
            EnemyCombatant *enemy;
            if (target == D_8009CD10 || target == actor) continue;
            enemy = target->core;
            if (enemy && enemy->hpAlive > 0 &&
                /* Retail tests actor here, not the candidate target. */
                target->teamId == actor->teamId && !(actor->entityFlags & 0x40000000)) break;
        }
    } else if ((actor->entityFlags & 0x6000) && ((EnemyCombatant *)actor->core)->field04.bytes.kind == 3) {
        for (target = (BattleEntity *)g_FieldActorListHead; target; target = target->next) {
            EnemyCombatant *enemy;
            if (target == D_8009CD10 || target == actor) continue;
            enemy = target->core;
            if (enemy && enemy->hpAlive > 0 &&
                target->parent == actor && enemy->field04.bytes.kind == 1) break;
        }
    } else if (!(actor->entityFlags & 0x4000)) target = actor;
    if (target) {
        unsigned int mode = g_ActiveActor->action->turnWord & 0xC0;
        if (mode == 0xC0 || mode == 0x40) {
            BattleInitSlot *slot;
            g_BattleRemainingActions = 0;
            slot = &g_BattleActionQueue[g_BattleActionQueueCount];
            slot->actor = target;
            slot->command = 2;
            slot->actionGeneration = g_BattleActiveTurnSlot;
            g_BattleActionQueueCount++;
        } else {
            while (g_BattleRemainingActions) {
                BattleInitSlot *slot;
                s16 actionGeneration = g_BattleActiveTurnSlot;
                g_BattleRemainingActions--;
                slot = &g_BattleActionQueue[g_BattleActionQueueCount];
                slot->actor = target;
                slot->command = 1;
                slot->actionGeneration = actionGeneration;
                g_BattleActionQueueCount++;
            }
        }
        Battle_DispatchEntityEffect();
        g_ActiveActor->stateFlags |= 0x200000;
    }
}

s8 g_BattleActiveTurnSlot, g_BattleItemMenuState, g_BattleTargetIndex;
u8 g_BattleRemainingActions, g_BattleActionQueueCount;

static inline void FinishAction(void) {
    u8 i;
    if ((unsigned int)((u16)g_BattleActionQueue[0].command - 3) < 404 && D_8009CD10->actionMode >= 4)
        g_GameStateFlags |= 0x100;
    if ((unsigned int)((u16)g_BattleActionQueue[0].command - 387) >= 21) {
        for (i=0; i<g_BattleActionQueueCount; i++) {
            if ((unsigned int)((u16)g_BattleActionQueue[i].command - 1) < 2) {
                Battle_DispatchEntityEffect();
                break;
            }
        }
    }
    g_BattleGaugeCapLatch = 0;
    g_ActiveActor->hpAlive = 0;
    g_ActiveActor->stateFlags |= 0x200000;
    BattleCmd_ResetTableCursor();
}
int Battle_HandleItemMenu(void) {
    u8 active=1;
    u8 opened=0, cancelled=0;
    if (!g_BattleActiveTurnSlot) {
        g_BattleRemainingActions=0;
        active=0;
        FinishAction();
        Battle_MarkActiveEntities(g_BattleTargetList,g_BattleTargetIndex);
        PLAY_TABLE_SOUND(0x44C)
        return active;
    } else {
        if ((g_FieldPadPressed&0x2000) && !MenuWidget_FindByModeAndSelectedBase(1,0) ) {
            cancelled=1;
            if (g_BattleItemMenuState==1) MenuWidget_InitPool();
            else if (g_BattleItemMenuState==2) BattleCmd_UndoPending();
            if (--g_BattleActiveTurnSlot) {
                g_BattleRemainingActions=g_ActiveActor->action->turnWord&15;
            } else {
                active=0;
                FinishAction();
                Battle_MarkActiveEntities(g_BattleTargetList,g_BattleTargetIndex);
                PLAY_TABLE_SOUND(0x44C)
                return active;
            }
            PLAY_TABLE_SOUND(0x44C)
        }
        if (!g_BattleItemMenuState) {
            if (g_BattleActiveTurnSlot) {
                int mode;
                if (g_ActiveActor->action->actionCode.actionId==8)mode=3;
                else {
                    unsigned int actionMode=(g_ActiveActor->action->turnWord>>6)&3;
                    switch(actionMode) {
                    case 1: mode=1; break;
                    case 3: mode=2; break;
                    default: mode=0; break;
                    }
                }
                Battle_CycleTarget(mode);
                if ((g_FieldPadPressed&0x200)&&!cancelled) {
                    if(g_BattleTargetCount) {
                        Battle_FillActionQueue(&g_BattleTargetList[g_BattleTargetIndex]);
                        if(!g_BattleRemainingActions) {
                            g_BattleActiveTurnSlot--;
                            g_BattleRemainingActions=g_ActiveActor->action->turnWord&15;
                        }
                    }
                    PLAY_TABLE_SOUND(0x44C)
                }
            }
            if(g_BattleRemainingActions==(g_ActiveActor->action->turnWord&15) && (g_FieldPadPressed&0x80) && g_BattleActiveTurnSlot && !cancelled) {
                g_BattleItemMenuState=1;
                Battle_MarkActiveEntities(g_BattleTargetList,g_BattleTargetIndex);
                PLAY_TABLE_SOUND(0x44C)
                Inv_OpenAyaItemList(1);
                opened=1;
                Render_BeginSceneLoad();
            }
            if ((g_FieldPadPressed&0x400)&&!opened&&!cancelled) {
                Battle_MarkActiveEntities(g_BattleTargetList,g_BattleTargetIndex);
                if(g_BattleActionQueueCount) {Battle_AdvanceTurnSlot();Battle_InitFadeVars();}
                else active=0;
                PLAY_TABLE_SOUND(0x44D)
            }
        } else {
            g_BattleItemMenuState=Battle_StepEnemyTurn(g_BattleItemMenuState);
            if(g_BattleItemMenuState==-1) {
                g_BattleItemMenuState=0;
                active=0;
                FinishAction();
            } else if(!g_BattleItemMenuState) Battle_InitFadeVars();
        }
    }
    return active;
}

u8 g_BattleActionQueueCount, g_BattleRemainingActions;
u8 g_AyaActionStep;
s8 g_BattleTargetCycleMode;
u16 g_BattlePendingCommand;
s8 g_BattleTargetIndex;

static inline void StoreSlot(BattleInitSlot *slot, BattleEntity *actor, s16 kind, s16 actionGeneration) {
    slot->actor = actor;
    slot->command = kind;
    slot->actionGeneration = actionGeneration;
}

static inline void SetSlot(int index, BattleEntity *actor, s16 kind, s16 actionGeneration) {
    StoreSlot(&g_BattleActionQueue[index], actor, kind, actionGeneration);
}

void Battle_FillActionQueue(BattleTarget *target) {
    unsigned int mode = g_ActiveActor->action->turnWord & 0xC0;
    if (mode == 0xC0) {
        u8 i;
        for (i = 0; g_BattleTargetList[i].actor; i++) {
            StoreSlot(&g_BattleActionQueue[g_BattleActionQueueCount], g_BattleTargetList[i].actor, 2, g_BattleActiveTurnSlot);
            g_BattleActionQueueCount++;
        }
        g_BattleRemainingActions = 0;
    } else if (mode == 0x40) {
        u8 i;
        for (i = 0; i < (int)(g_ActiveActor->action->turnWord & 15) * 3 / 2; i++) {
            int index = g_BattleActionQueueCount;
            int randomValue = rand();
            BattleEntity *actor = g_BattleTargetList[randomValue % g_BattleTargetCount].actor;
            SetSlot(index, actor, 2, g_BattleActiveTurnSlot);
            g_BattleActionQueueCount++;
        }
        g_BattleRemainingActions = 0;
    } else {
        SetSlot(g_BattleActionQueueCount, target->actor, 1, g_BattleActiveTurnSlot);
        g_BattleActionQueueCount++;
        g_BattleRemainingActions--;
    }
}

int Battle_StepEnemyTurn(s8 mode) {
    if (mode == 1) {
        int command = g_MenuFrameCommand;
        s16 queuedCommand = g_MenuFrameCommand;
        if (command > 0) {
            if (command < 387) {
                SetSlot(g_BattleActionQueueCount, D_8009CD10, queuedCommand, g_BattleActiveTurnSlot);
                g_BattleActionQueueCount++;
                g_BattleActiveTurnSlot--;
                g_BattleTargetCycleMode = 0;
                mode = 0;
            } else if (command < 407) {
                switch (command) {
                case 393:
                    Battle_InitFadeVars();
                    g_BattleTargetCycleMode = 4;
                    mode++;
                    break;
                case 394:
                    Battle_InitFadeVars();
                    g_BattleTargetCycleMode = 5;
                    mode++;
                    break;
                case 395:
                    Battle_InitFadeVars();
                    g_BattleTargetCycleMode = 6;
                    mode++;
                    break;
                case 397:
                    Battle_InitFadeVars();
                    g_BattleTargetCycleMode = 7;
                    mode++;
                    break;
                case 406:
                    Battle_InitFadeVars();
                    g_AyaActionStep = 0;
                    for (mode = 0; (u8)mode < 7; mode++) {
                        int index = g_BattleActionQueueCount;
                        int randomValue = rand();
                        SetSlot(index, g_BattleTargetList[randomValue % g_BattleTargetCount].actor,
                                g_MenuFrameCommand, g_BattleActiveTurnSlot);
                        g_BattleActionQueueCount++;
                    }
                    g_BattleActiveTurnSlot--;
                    g_BattleTargetCycleMode = 0;
                    mode = 0;
                    break;
                default:
                    SetSlot(g_BattleActionQueueCount, D_8009CD10, g_MenuFrameCommand, g_BattleActiveTurnSlot);
                    g_BattleActiveTurnSlot--;
                    g_BattleActionQueueCount++;
                    g_BattleTargetCycleMode = 0;
                    mode = 0;
                    break;
                }
            } else if (command < 409) {
                SetSlot(g_BattleActionQueueCount, D_8009CD10, queuedCommand, g_BattleActiveTurnSlot);
                if (!(g_GameStateFlags & 2)) {
                    g_ActiveActor = D_8009CD10->core;
                    Asset_Find08Alt(0x453, 1, D_8009CD10->posX.parts.integer,
                                    D_8009CD10->posY.parts.integer,
                                    D_8009CD10->posZ.parts.integer);
                }
                switch (command) {
                case 407:
                    Battle_CopyPadStateToRecord();
                    Battle_SetupEnemyAnims();
                    g_ActiveActor->actionMode12 = 13;
                    break;
                case 408:
                    Battle_SyncEnemyAttributes();
                    break;
                }
                g_BattleActionQueueCount++;
                g_BattleActiveTurnSlot = 0;
                g_BattleTargetCycleMode = 0;
                mode = -1;
            } else {
                SetSlot(g_BattleActionQueueCount, D_8009CD10, queuedCommand, g_BattleActiveTurnSlot);
                g_BattleActionQueueCount++;
                g_BattleActiveTurnSlot = 0;
                g_BattleTargetCycleMode = 0;
                mode = -1;
            }
            if (!g_BattleTargetCycleMode)
                PLAY_TABLE_SOUND(0x44C)
            g_BattlePendingCommand = g_MenuFrameCommand;
        } else if (command == -1) {
            mode = 0;
        }
    } else if (mode == 2) {
        Battle_CycleTarget(g_BattleTargetCycleMode);
        if (g_FieldPadPressed & 0x200) {
            SetSlot(g_BattleActionQueueCount, g_BattleTargetList[g_BattleTargetIndex].actor, g_BattlePendingCommand,
                    g_BattleActiveTurnSlot);
            g_BattleActiveTurnSlot--;
            g_BattleActionQueueCount++;
            g_BattleTargetCycleMode = 0;
            mode = 0;
            PLAY_TABLE_SOUND(0x44C)
        } else if (g_FieldPadPressed & 0x400) {
            mode = 0;
            BattleCmd_UndoPending();
            g_BattleTargetIndex = 0;
            PLAY_TABLE_SOUND(0x44D)
        }
    }
    return mode;
}

/* COMMON supplies small-data metadata to MASPSX. */
u8 g_BattleActionQueueCount;
u8 g_BattleRemainingActions;

/* Step the turn index back to the first slot of the action that ends at
 * initial. This revision has no slot-zero guard: the first slot always
 * starts a new action generation. The halfword parameter makes the rewind
 * compute its own initial - 1 instead of reusing the one from the command
 * lookup. */
static inline void RewindTurn(s16 initial)
{
    g_BattleActionQueueCount = initial - 1;
    while (g_BattleActionQueue[g_BattleActionQueueCount].actionGeneration ==
           g_BattleActionQueue[g_BattleActionQueueCount - 1].actionGeneration)
        g_BattleActionQueueCount--;
}

void Battle_AdvanceTurnSlot(void)
{
    int savedIndex = g_BattleActionQueueCount;
    int kind;
    s16 i;
    s8 generation;
    g_BattleTargetIndex = 0;
    kind = g_BattleActionQueue[savedIndex - 1].command;
    if (kind == 1) {
        if (g_BattleRemainingActions == (g_ActiveActor->action->turnWord & 15)) {
            g_BattleActiveTurnSlot++;
            RewindTurn(savedIndex);
        } else {
            RewindTurn(savedIndex);
        }
        g_BattleRemainingActions = g_ActiveActor->action->turnWord & 15;
    } else if (kind == 2) {
        int mode = g_ActiveActor->action->turnWord & 0xC0;
        if (mode == 0xC0) {
            g_BattleActiveTurnSlot++;
            g_BattleActionQueueCount = savedIndex - (u8)g_BattleTargetCount;
        } else if (mode == 0x40) {
            int word;
            g_BattleActiveTurnSlot++;
            word = g_ActiveActor->action->turnWord;
            g_BattleActionQueueCount = savedIndex - (((word & 15) * 3) >> 1);
        }
        g_BattleRemainingActions = g_ActiveActor->action->turnWord & 15;
    } else if (kind < 0x197) {
        g_BattleActiveTurnSlot++;
        BattleCmd_UndoPending();
        do {
            g_BattleActionQueueCount--;
        } while (g_BattleActionQueue[g_BattleActionQueueCount].actionGeneration ==
                 g_BattleActionQueue[g_BattleActionQueueCount - 1].actionGeneration);
        g_BattleRemainingActions = g_ActiveActor->action->turnWord & 15;
    }
    generation = g_BattleActiveTurnSlot;
    for (i = 0; i < 45; i++) {
        if (g_BattleActionQueue[i].actionGeneration == generation) {
            g_BattleActionQueue[i].actor = 0;
            (&g_BattleActionQueue[i])->actionGeneration = 0;
            g_BattleActionQueue[i].command = 0;
        }
    }
}

/* Shared level and direction for the pulsing target highlight. Retail linker
 * objects own the storage; these retain the matching COMMON metadata. */
u8 g_BattleFadeLevel;
s8 g_BattleFadeStep;

void Battle_InitFadeVars(void) {
    if (g_BattleTargetCount != 0) {
        g_BattleFadeLevel = 0x80;
        g_BattleFadeStep = -8;
    }
}

/* Fade the targeted actor; group members (kind 4) fade together, linked parts
 * (kind 1) fade their parent. The level is evaluated at the call. */
#define FADE_TARGET(target, level)                                                       \
    {                                                                                    \
        BattleEntity *entity = (target);                                                 \
        BattleEntity *actor;                                                             \
        int kind = ((EntityAnimEventCore *)entity->core)->kind;               \
        if (kind == 1) {                                                                 \
            Render_FadeEntityColor(&entity->parent->renderObject, (level), (level),      \
                                   (level));                                             \
        } else if (kind == 4) {                                                          \
            for (actor = (BattleEntity *)g_FieldActorListHead; actor != 0; actor = actor->next) {                  \
                if (actor != D_8009CD10 && actor->core != 0 &&                           \
                    ((Combatant *)actor->core)->field04.bytes.kind == 4) {        \
                    Render_FadeEntityColor(&actor->renderObject, g_BattleFadeLevel,      \
                                           g_BattleFadeLevel, g_BattleFadeLevel);        \
                }                                                                        \
            }                                                                            \
        } else {                                                                         \
            Render_FadeEntityColor(&entity->renderObject, (level), (level), (level));    \
        }                                                                                \
    }

void Battle_StepPlayerTurn(BattleTarget *targets, int index, int mode)
{
    u8 level;
    s16 center;
    s16 lower;
    s16 upper;
    s16 angle;
    int center_copy, low_copy, high_copy, inside;
    u8 slot;
    int targetMode;

    if (g_BattleTargetCount == 0) {
        return;
    }
    if (g_BattleFadeLevel < 0x41) {
        g_BattleFadeStep = 8;
    } else if (g_BattleFadeLevel >= 0xC0) {
        g_BattleFadeStep = -8;
    }
    level = g_BattleFadeLevel + g_BattleFadeStep;
    g_BattleFadeLevel = level;

    if ((s8)mode < 4) {
        targetMode = (g_ActiveActor->action->turnWord >> 6) & 3;
        switch (targetMode) {
        case 0:
            FADE_TARGET(targets[(s8)index].actor, level);
            break;
        case 2:
            center = targets[(s8)index].angle;
            if (center < -0x600) {
                lower = center + 0x200;
                upper = center + 0xE00;
            } else if (center < 0x600) {
                lower = center - 0x200;
                upper = center + 0x200;
            } else {
                lower = center - 0xE00;
                upper = center - 0x200;
            }
            if (targets[0].actor == 0) {
                return;
            }
            slot = 0;
            high_copy = upper;
            low_copy = lower;
            center_copy = center;
            inside = center < 0x600;
            for (; targets[slot].actor != 0; slot++) {
                if (center_copy >= -0x600 && inside) {
                    angle = targets[slot].angle;
                    if (angle < low_copy || angle > high_copy) {
                        continue;
                    }
                } else {
                    angle = targets[slot].angle;
                    if (angle < high_copy && angle > low_copy) {
                        continue;
                    }
                }
                FADE_TARGET(targets[slot].actor, g_BattleFadeLevel);
            }
            break;
        case 1:
        case 3:
            for (slot = 0; targets[slot].actor != 0; slot++) {
                FADE_TARGET(targets[slot].actor, g_BattleFadeLevel);
            }
            break;
        }
    } else if ((s8)mode < 8) {
        FADE_TARGET(targets[(s8)index].actor, level);
    }
}

/* MASPSX_FORCE_G0 keeps the battle globals absolute; the COMMON fade step
 * g_BattleFadeStep supplies the small-data metadata of the one $gp-relative byte. */
s8 g_BattleFadeStep;

inline static void Mark(BattleEntity *entity)
{
  int kind = ((Combatant *)entity->core)->field04.bytes.kind;
  if (kind == 1)
  {
    entity = entity->parent;
  }
  else
    if (kind == 4)
  {
    BattleEntity *active;
    entity = (BattleEntity *)g_FieldActorListHead;
    if (!entity)
      return;
    active = D_8009CD10;
    while (entity != 0)
    {
      if (((entity != active) && ((entity->core) != 0)) && ((((Combatant *)entity->core)->field04.bytes.kind) == 4))
      {
        entity->renderObject.flags_9C |= 0x20;
      }
      entity = entity->next;
    }

    return;
  }
  entity->renderObject.flags_9C |= 0x20;
}

inline static void MarkGroup(BattleEntity *entity, int group_kind, BattleEntity *active)
{
  int kind = ((Combatant *)entity->core)->field04.bytes.kind;
  if (kind == 1) entity = entity->parent;
  else if (kind == group_kind) {
    entity = (BattleEntity *)g_FieldActorListHead;
    while (entity != 0) {
      if (entity != active && entity->core != 0 && ((Combatant *)entity->core)->field04.bytes.kind == group_kind)
        entity->renderObject.flags_9C |= 0x20;
      entity = entity->next;
    }
    return;
  }
  entity->renderObject.flags_9C |= 0x20;
}

inline static void MarkKinds(BattleEntity *entity, int group_kind, int linked_kind, BattleEntity *active)
{
  int kind = ((Combatant *)entity->core)->field04.bytes.kind;
  if (kind == linked_kind) entity = entity->parent;
  else if (kind == group_kind) {
    entity = (BattleEntity *)g_FieldActorListHead;
    while (entity != 0) {
      if (entity != active && entity->core != 0 && ((Combatant *)entity->core)->field04.bytes.kind == group_kind)
        entity->renderObject.flags_9C |= 0x20;
      entity = entity->next;
    }
    return;
  }
  entity->renderObject.flags_9C |= 0x20;
}

void Battle_MarkActiveEntities(BattleTarget *table, int selected_index)
{
  int mode;
  s16 center;
  s16 lower;
  s16 upper;
  int center_copy;
  int low_copy, high_copy;
  int inside;
  int linked_kind;
  int group_kind;
  int range_group;
  BattleEntity *cached_active;
  unsigned char slot;
  BattleEntity *active;
  if (g_BattleTargetCount == 0)
  {
    return;
  }
  mode = (g_ActiveActor->action->turnWord >> 6) & 3;
  g_BattleFadeStep = 0;
  switch (mode)
  {
    case 0:
      Mark(table[(s8) selected_index].actor);
      break;

    case 2:
      center = table[(s8) selected_index].angle;

      if (center < (-0x600))
    {
      lower = center + 0x200;
      upper = center + 0xE00;
    }
    else
      if (center < 0x600)
    {
      lower = center - 0x200;
      upper = center + 0x200;
    }
    else
    {
      lower = center - 0xE00;
      upper = center - 0x200;
    }
      if (table[0].actor == 0)
    {
      return;
    }
      slot = 0;
      high_copy = upper;
      low_copy = lower;
      center_copy = center;
      inside = center < 0x600;
      range_group = 4;
      active = D_8009CD10;
      cached_active = active;
      for (; table[slot].actor != 0; slot++)
    {
      s16 angle;
      if ((center_copy >= (-0x600)) && inside)
      {
        angle = table[slot].angle;
        if ((angle < low_copy) || (angle > high_copy))
        {
          continue;
        }
      }
      else
      {
        angle = table[slot].angle;
        if ((angle < high_copy) && (angle > low_copy))
        {
          continue;
        }
      }
      MarkGroup(table[slot].actor, range_group, cached_active);
    }

      break;

    case 1:

    case 3:
      if (table[0].actor == 0)
    {
      return;
    }
      slot = 0;
      linked_kind = 1;
      group_kind = 4;
      /* Retail materializes both kinds before loading the active actor. */
            active = D_8009CD10;
      for (; table[slot].actor != 0; slot++)
    {
      MarkKinds(table[slot].actor, group_kind, linked_kind, active);
    }

      break;

  }

}

/* MASPSX_FORCE_G0 keeps the battle words absolute, as retail; the COMMON
 * fade level supplies the small-data metadata of the one $gp-relative byte. */
u8 g_BattleFadeLevel;

int Battle_ProcessActionSlot(BattleEntity *actor)
{
    EnemyCombatant *state = actor->core;
    int phase = state->coreFlags & 0x6000;
    s8 result = 1;

    if (phase == 0x2000) {
        int index = g_BattleActionIndex;
        s16 command = g_BattleActionQueue[index].command;
        if (command != 0x196 && command != 0x189) {
            if (g_ActiveActor->action->actionCode.actionId != 6) {
                if (g_BattleActionActorCount == 0 && g_BattleActionActors[0] != 0) {
                    int i = 0;
                    do {
                        g_BattleActionActors[(u8)i] = 0;
                        i++;
                    } while (g_BattleActionActors[(u8)i] != 0);
                }
                {
                    u8 count = g_BattleActionActorCount;
                    g_BattleActionActorCount = count + 1;
                    g_BattleActionActors[count] = actor;
                }
            } else {
                /* Matching debt: retail sets the room argument before the
                 * test, in the branch delay slot. */
                register int room asm("$4") = 0x5B;
                if (actor == g_BattleActionQueue[index].actor) {
                    g_BattleAilmentPmSlot = Scene_LoadRoomAssets(room, actor);
                    g_BattleActionActorCount = 0;
                }
            }
        }
        result = 0;
        if (state->field04.bytes.kind != 0 && state->hpAlive > 0) {
            g_BattleFadeLevel = 0xFF;
        }
    } else if (phase == 0x4000 && state->field04.bytes.kind != 0) {
        u8 color = g_BattleFadeLevel - 8;
        g_BattleFadeLevel = color;
        if (state->field04.bytes.kind == 1) {
            if (color <= 0x80) {
                actor->parent->renderObject.flags_9C |= 0x20;
                state->coreFlags &= ~0x6000;
            } else {
                Render_FadeEntityColor(&actor->parent->renderObject, color, color, color);
                result = 0;
            }
        } else if (state->field04.bytes.kind == 4) {
            BattleEntity *iter;
            for (iter = (BattleEntity *)g_FieldActorListHead; iter != 0; iter = iter->next) {
                if (iter != D_8009CD10 && iter->core != 0 &&
                    ((EnemyCombatant *)iter->core)->field04.bytes.kind == 4) {
                    int shade = g_BattleFadeLevel;
                    if ((u8)shade <= 0x80) {
                        iter->renderObject.flags_9C |= 0x20;
                        state->coreFlags &= ~0x6000;
                    } else {
                        Render_FadeEntityColor(&iter->renderObject, shade, shade, shade);
                        result = 0;
                    }
                }
            }
            return result;
        } else {
            if (state->hpAlive > 0) {
            if (color <= 0x80) {
                actor->renderObject.flags_9C |= 0x20;
                state->coreFlags &= ~0x6000;
            } else {
                Render_FadeEntityColor(&actor->renderObject, color, color, color);
                result = 0;
            }
            }
        }
    }
    return result;
}

/* Advance enemy charge/status timers, movement, damage panels and death state.
* The core is read twice, so retail keeps a second copy of it (statusCore).
* Matching debt: the actor barrier gives the entity s0 and its core s1;
* without it global allocation swaps them (lev 129).
* curHP is the existing name for the 0..9000 charge gauge in this core view.
*/
void Battle_UpdateEnemy(BattleEntity *entity) {
    BattleEntity *actor = entity;
    EnemyCombatant *statusCore;
    BattleEntity *parent;
    BattleEntity *livingLink;
    BattleEntity *linkedActor;
    EnemyCombatant *linkedCore;
    EnemyCombatant *enemy;
    EnemyCombatant *otherCore;
    s16 angle;
    s32 hpChange;
    s32 actionPhase;
    s32 damageTicks;
    s32 advancedAngle;
    s32 wrappedAngle;
    EnemyActionEffect *expiredEffect;
    EnemyActionEffect *expiredStatusEffect;
    s8 linkedKindValue;
    s8 effectKind;
    s8 deadKind;
    u16 nextCharge;
    u16 charge;
    u32 initialFlags;
    u32 statusFlags;
    u32 damageFlags;
    u32 nextFlags;
    u32 nextStatusFlags;
    u32 actionFlags;

    asm("" : "=r"(actor) : "0"(actor));
    enemy = actor->core;
    statusCore = actor->core;
    {
        s32 hp = enemy->hpAlive;
        s32 maximum = enemy->hpAliveCap;
        if (maximum < hp)
            enemy->hpAlive = maximum;
    }
    if (!(g_GameStateFlags & 0x100)) {
        if (enemy->curHP == 0) {
            initialFlags = enemy->coreFlags;
            if (initialFlags & 0xE) {
                u32 preserved = initialFlags & ~0xE;
                u32 count = ((((initialFlags >> 1) & 7) - 1) & 7) * 2;
                nextFlags = preserved | count;
                enemy->coreFlags = nextFlags;
                if (!(nextFlags & 0x180E)) {
                    actor->entityFlags &= ~0x1000;
                    Entity_TickAnimSequences(actor);
                    expiredEffect = enemy->effect;
                    if (expiredEffect != 0) {
                        expiredEffect->state = 4;
                        enemy->coreFlags &= 0xC0FFFFFF;
                    }
                }
            }
            statusFlags = statusCore->coreFlags;
            if (statusFlags & 0x1800) {
                nextStatusFlags = (statusFlags & ~0x1800) | (((((statusFlags >> 0xB) & 3) - 1) & 3) << 0xB);
                statusCore->coreFlags = nextStatusFlags;
                if (!(nextStatusFlags & 0x180E)) {
                    actor->entityFlags &= ~0x1000;
                    Entity_TickAnimSequences(actor);
                    expiredStatusEffect = enemy->effect;
                    if (expiredStatusEffect != 0) {
                        expiredStatusEffect->state = 4;
                        statusCore->coreFlags &= 0xC0FFFFFF;
                    }
                }
            }
        }
        charge = enemy->curHP;
        if (charge < 0x2328U) {
            nextCharge = charge + enemy->chargeStep;
            enemy->curHP = nextCharge;
            if (statusCore->coreFlags & 1) {
                enemy->curHP = nextCharge - ((enemy->chargeStep * 2) / 5);
            }
        } else {
            enemy->curHP = 0x2328;
            if (actor->entityFlags & 0x1000) {
                enemy->curHP = 0;
            }
        }
        damageFlags = statusCore->coreFlags;
        if (damageFlags & 0x10) {
            damageTicks = (damageFlags >> 5) & 0x1F;
            if (damageTicks >= 0x1E) {
                enemy->hpAlive -= enemy->periodicDamage;
                statusCore->coreFlags &= ~0x3E0;
            } else {
                statusCore->coreFlags = (damageFlags & ~0x3E0) | (((damageTicks + 1) & 0x1F) << 5);
            }
        }
    }
    if (statusCore->coreFlags & 0x6000) {
        Battle_ProcessActionSlot(actor);
        actionFlags = statusCore->coreFlags;
        actionPhase = actionFlags & 0x6000;
        if (actionPhase == 0x2000) {
            Battle_StepEnemyMovement(actor);
            Asset_Find08w(enemy->attackAssetId, 0, actor->renderObject.target_x, actor->renderObject.target_y, actor->renderObject.target_z);
        } else if (actionPhase == 0x4000) {
            s32 kind = enemy->field04.bytes.kind;
            if (kind == 0 && (actionFlags & 0xE) == 0) {
                {
                    if (actor->animLastFrame == actor->animPrev.parts.integer) {
                        if ((enemy->resumePhase == 2) && !(enemy->coreFlags & 0x1800)) {
                            enemy->resumePhase--;
                            Entity_SetActionMode(actor, enemy->savedActionMode);
                            if (enemy->savedInterpolation != 0) {
                                actor->entityFlags |= 0x200;
                            }
                            actor->animFrame = enemy->savedAnimFrame;
                            actor->animPrev.fixed = enemy->savedAnimPrev;
                            actor->animStep = enemy->savedAnimStep;
                        } else {
                            {
                                BattleEntity *callEntity;
                                unsigned mode = enemy->lowAndEntityId.bytes.low;
                                callEntity = actor;

                                Entity_SetActionMode(callEntity, (u16)(s8)mode);
                            }
                        }
                        statusCore->coreFlags &= ~0x6000;
                        if (!(g_ActiveActor->stateFlags & 0x80000) && !(enemy->coreFlags & 0x1800)) {
                            actor->entityFlags &= ~0x1000;
                            Entity_TickAnimSequences(actor);
                        }
                    } else if ((enemy->motionPhase >= 2U) && (enemy->motionFrames != 0)) {
                        actor->posX.fixed += enemy->motionAmplitude * rsin(enemy->motionAngle) * 0x10;
                        actor->posZ.fixed += enemy->motionAmplitude * rcos(enemy->motionAngle) * 0x10;
                        enemy->motionFrames -= 1;
                    }
                }
            } else {
                if ((actionFlags & 0xE) != 0) {
                    statusCore->coreFlags &= ~0x6000;
                    if ((enemy->field04.bytes.kind != 0) && (enemy->hpAlive > 0)) {
                        effectKind = ((EnemyCombatant *)actor->core)->field04.bytes.kind;
                        if (effectKind == 1) {
                            parent = actor->parent;
                            parent->renderObject.flags_9C |= 0x20;
                        } else if (effectKind == 4) {
                            int linkedKind = 4;
                            BattleEntity *player;
                            linkedActor = (BattleEntity *)g_FieldActorListHead;
                            if (linkedActor != 0) {
                                player = D_8009CD10;
                                do {
                                    if (linkedActor != player) {
                                        otherCore = linkedActor->core;
                                        if ((otherCore != 0) && (otherCore->field04.bytes.kind == linkedKind)) {
                                            linkedActor->renderObject.flags_9C |= 0x20;
                                        }
                                    }
                                    linkedActor = linkedActor->next;
                                } while (linkedActor != 0);
                            }
                        } else {
                            actor->renderObject.flags_9C |= 0x20;
                        }
                    }
                }
            }
        }
        if (enemy->field04.bytes.kind != 1) {
            actor->motionX = 0;
            actor->motionY = 0;
            actor->motionZ = 0;
        }
    }
    if ((statusCore->coreFlags & 0xE) && (enemy->hpAlive > 0)) {
        int contextFlags = g_GameStateFlags;
        actor->motionX = 0;
        actor->motionY = 0;
        actor->motionZ = 0;
        if (!(contextFlags & 0x100)) {
            angle = actor->facingAngle;
            advancedAngle = angle + 0x80;
            wrappedAngle = advancedAngle;
            if (advancedAngle < 0) {
                wrappedAngle = angle + 0x107F;
            }
            actor->facingAngle = advancedAngle - ((wrappedAngle >> 0xC) << 0xC);
        }
    }
    if (statusCore->coreFlags & 0x1800) {
        actor->motionX = 0;
        actor->motionY = 0;
        actor->motionZ = 0;
    }
    if (statusCore->coreFlags & 0x400) {
        enemy->hpAlive = -1;
    }
    hpChange = enemy->previousHP - enemy->hpAlive;
    if (hpChange != 0) {
        if (hpChange < 0) {
            enemy->panelC_val = -hpChange;
            enemy->panelC_mode = 1;
        } else {
            enemy->panelC_val = hpChange;
            if ((statusCore->coreFlags & 0x38000) == 0x18000) {
                enemy->panelC_mode = 2;
            } else {
                enemy->panelC_mode = 0;
            }
        }
        enemy->panelC_x = (u16) actor->renderObject.projected_target_x;

        enemy->panelC_y = (u16) actor->renderObject.projected_target_y - 0x14;
        enemy->panelC_timer = 0x1E;
    }
    if (enemy->panelC_timer != 0) {
        Battle_DrawStatusPanel(1, (BattleStatusPanel *)&enemy->panelC_val);
        enemy->panelC_timer -= 1;
    }
    if (enemy->hpAlive <= 0) {
        if (!(g_GameStateFlags & 0x100)) {
            if ((enemy->field04.bytes.kind == 1) && !(enemy->coreFlags & 0x6000)) {
                actor->entityFlags |= 0x10;
                Battle_SlotFree(actor);
            }
            if (enemy->field04.bytes.kind == 4) {
                if (!(enemy->coreFlags & 0x6000)) {
                    actor->motionX = 0;
                    actor->motionY = 0;
                    actor->motionZ = 0;
                    Battle_StepEntityAnimState(actor);
                }
            } else if (enemy->field04.bytes.kind != 1 && enemy->field04.bytes.kind != 3) {
                /* Matching debt: retail reaches this step from the kind-3
                 * search below; a copy there is laid out as its own block
                 * (lev 19). */
stepDeathAnimation:
                actor->motionX = 0;
                actor->motionY = 0;
                actor->motionZ = 0;
                Battle_StepEntityAnimState(actor);
            } else {
                livingLink = (BattleEntity *)g_FieldActorListHead;
                for (;;) {
                    if (livingLink == 0) {
                        deadKind = enemy->field04.bytes.kind;
                        if (deadKind != 3) {
                            if (deadKind == 1) {
                                actor->entityFlags |= 0x10;
                                Battle_SlotFree(actor);
                                actor->parent->motionX = 0;
                                actor->parent->motionY = 0;
                                actor->parent->motionZ = 0;
                                Battle_StepEntityAnimState(actor->parent);
                            }
                        } else {
                            goto stepDeathAnimation;
                        }
                        break;
                    } else {
                        linkedCore = livingLink->core;
                        if ((linkedCore == 0) || (livingLink == D_8009CD10) || (linkedKindValue = linkedCore->field04.bytes.kind, (linkedKindValue == 0)) || (linkedKindValue == 2) || (linkedKindValue == 4) || (linkedCore->hpAlive <= 0)) {
                            livingLink = livingLink->next;
                            continue;
                        }
                    }
                    break;
                }
            }
        }
    }
    enemy->previousHP = enemy->hpAlive;
}
