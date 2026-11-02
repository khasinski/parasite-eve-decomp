/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
/* ASPSX_VERSION: 2.56 */

#include "common.h"
#include "pe1/akao/commands.h"
#include "pe1/aya.h"
#include "pe1/battle.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_level_up.h"
#include "pe1/battle_modifiers.h"
#include "pe1/battle_palette.h"
#include "pe1/battle_reset_stats.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_saved_state.h"
#include "pe1/battle_start.h"
#include "pe1/battle_status.h"
#include "pe1/battle_turn.h"
#include "pe1/battle_update.h"
#include "pe1/game_state.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
u8 g_BattleSuspendedModeState;
void *g_BattlePendingEnemySpawn;
s32 g_BattleFrameCounter;
u8 g_BattleActionActorCount;
u8 g_BattleEnemyEscapeFlag;
Combatant *g_ActiveActor;
u8 g_BattleGaugeCapLatch;
int g_BattleModeState;
s8 g_BattleLiveEnemyCount;
s16 g_MenuFrameCommand;
s8 g_BattleTargetCount;
s32 g_BattleImpactPmSlot;
u8 g_BattleStepIndex;
u32 g_BattleExpReward;
u16 g_BattlePeBonusReward;
s32 g_BattleModeState;
BattleAttributes g_BattleEquipStateBlock;

void Battle_SaveAyaState(void);

void Save_InitSystem(void)
{
    u8 i;

    Battle_SaveAyaState();
    for (i = 0; i < 7; i++) {
        g_BattleEnemySlots[i].active = 0;
    }
    g_BattleLiveEnemyCount = 0;
    g_EntityInstanceCounter = 0;
    Save_ResetGlobalFlags();
}

/* COMMON supplies small-data metadata to MASPSX; the other battle words
 * stay absolute. The story-day flags byte is set through its g_GameState
 * record field: the record is not small data, so its address stays in a
 * register for the read-modify-write, as retail has it. */
u8 g_BattleStepIndex, g_BattleStartClearedByteWriteOnly, g_BattleSuspendedModeState;
void *g_BattlePendingEnemySpawn;
int g_BattleFrameCounter;
u16 g_BattlePeBonusReward;
u8 g_BattleEnemyEscapeFlag;
Combatant *g_ActiveActor;
u8 g_BattleGaugeCapLatch;
int g_BattleModeState;
s8 g_BattleLiveEnemyCount;
u8 g_EntityInstanceCounter;
u32 g_BattleExpReward;

void Battle_ResetEnemyStats(int mode) {
    Combatant *actor;
    Combatant *clear_actor;
    u32 clear_flags;

    actor = g_ActiveActor;
    if ((s16)actor->maxHP < (s16)actor->curHP) {
        actor->curHP = actor->maxHP;
    }

    g_ActiveActor->actionMode12 = 4;
    actor = g_ActiveActor;
    actor->hpAlive = 0;
    actor->atbGauge = 0;
    actor->hpMirror = actor->curHP;
    g_BattlePendingEnemySpawn = 0;

    if ((mode & 0xFF) == 1) {
        g_BattleEnemyHoldTimer[0] = 0x5A;
        g_BattleEnemyEscapeFlag = 1;
        actor->stateFlags |= 0x800000;
    } else {
        u32 state = actor->stateFlags;
        g_BattleEnemyEscapeFlag = 0;
        state &= 0xFFBFFFFF;
        state &= 0xFF7FFFFF;
        actor->stateFlags = state;
    }

    clear_actor = g_ActiveActor;
    clear_flags = clear_actor->stateFlags;
    clear_actor->panelA_timer = 0;
    clear_flags &= -4;
    clear_flags &= -0xD;
    clear_flags &= -0x31;
    clear_flags &= -0xC1;
    clear_flags &= -0x101;
    clear_flags &= -0x201;
    clear_flags &= -0x401;
    clear_flags &= -0x801;
    clear_flags &= -0x1001;
    clear_flags &= -0x2001;
    clear_flags &= -0x4001;
    clear_flags &= 0xFFFF7FFF;
    clear_flags &= 0xFFF7FFFF;
    clear_flags &= 0xFFFEFFFF;
    clear_flags &= 0xFFF9FFFF;
    clear_flags &= 0xFFEFFFFF;
    clear_flags &= 0xFFDFFFFF;
    clear_flags &= 0xFEFFFFFF;
    clear_flags &= 0xF1FFFFFF;
    clear_flags &= 0xEFFFFFFF;
    clear_flags &= 0xDFFFFFFF;
    clear_actor->stateFlags = clear_flags;

    g_ActiveActor->panelB_timer = 0;
    g_ActiveActor->panelAux_timer = 0;
    D_8009CDA8 &= ~0x10;
    Battle_FlushScriptSounds();
    Draw_ResetTextboxes();
    g_BattleHitActionPending = 0;
    g_BattleHitActionMode[0] = 0;
    g_BattleHitActionSubmode[0] = 0;
    g_BattleHitActionParam[0] = 0;
}

/* Reset the PE gauge fill and remainder gradients of both draw buffers. */
void Battle_SetupPlayerPalette(void) {
    u8 *flags;

    BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
    BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
    BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][1], 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
    BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][1], 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
    Battle_FlushScriptSounds();
    g_EntityInstanceCounter = 0;
    g_BattleLiveEnemyCount = 0;
    D_8009CD10->actionCheckFn = (void *)g_FieldActorTypeCallbacks[0].contact;
    g_GameStateFlags &= -3;
    D_8009CDA8 &= -0x11;
    flags = &g_GameState.story_day_flags;
    *flags |= 2;
    BattleCmd_SyncActiveAmmo();
}

void Battle_StartEncounter(int mode) {
    int i;
    int neg_one;
    BattleEntity *entity;
    Combatant *actor;
    s32 value;
    int mode_reg;
    u32 initial_flags;
    entity = D_8009CD10;
    g_ActiveActor = entity->core;
    g_BattleFrameCounter = 0;
    g_BattleAttackAnimFrame = 0;
    g_BattleModeState = 0;
    g_BattleSuspendedModeState = 0;
    g_BattleStartClearedByteWriteOnly = 0;
    g_BattleGaugeCapLatch = 0;
    g_BattleStepIndex = 0;
    mode_reg = mode;
    Save_ResetGlobalFlags();

    i = 0;
    neg_one = -1;
    *(u8 *)&g_BattleResultFlags = 0;
    initial_flags = g_BattleResultFlags;
    g_BattleScanPanel.summary = 0;
    g_BattleSaveOverlayActive = 0;
    g_BattleSpritePrimCountdown = 0;
    g_BattleExpReward = 0;
    g_BattlePeBonusReward = 0;
    initial_flags &= -0x301;
    g_BattleResultFlags = initial_flags;

    for (; (u8)i < 0xA; i++) {
        g_BattleRewardSlots[(u8)i].id = 0;
        g_BattleRewardSlots[(u8)i].extra = neg_one;
    }

    for (i = 0; (u8)i < 7; i++) {
        g_BattleActionActors[(u8)i] = 0;
    }

    srand(g_FrameRngCounter);
    Battle_ResetEnemyStats(0);
    Battle_SetupEnemyAnims();

    actor = g_ActiveActor;
    value = actor->exp_or_acc;
    if (value <= 0) {
        actor->exp_or_acc = 0x10000;
    } else {
        s32 max_value;

        max_value = actor->maxAtk;
        if (value >= max_value) {
            actor->exp_or_acc = max_value;
            actor->atbGauge = 0xF0;
        }
    }

    Battle_CheckDropChance();
    {
        BattleEntity *post_entity;
        RenderMatrix *post_actor;
        void *callback;
        post_entity = D_8009CD10;
        post_actor = post_entity->renderObject.matrices;
        callback = Entity_CheckActionIdMatch;
        post_entity->actionCheckFn = callback;
        g_BattlePlayerAimHeight = post_actor->translation[1] - 0x64;
    }
    Battle_SetWindowBoundsByMode(mode_reg & 0xFF);
    {
        Combatant *final_actor;
        void *final_entity;
        final_actor = g_ActiveActor;
        final_entity = D_8009CD10;
        Entity_SetActionMode(final_entity, final_actor->actionMode12);
    }
}

void Battle_Update(void) {
    /* Retail reserves 0x1A0 unused bytes; original local layout is unknown. */
    volatile u8 matchingStackReserve[0x1A0];
    s32 menuMode;
    BattleEntity *player;
    Combatant *active;
    s8 menuResult;
    int phase;
    int opened;
    u8 previousPhase;
    Combatant *status;
    BattleEntity *entity;
    BattleEntity *victoryEntity;

    menuMode = 1;
    if (g_ActiveActor->stateFlags & 0x80000) {
        if (g_BattleModeState == 6) {
            g_BattleSuspendedModeState = 6;
            g_BattleModeState = 0;
        }
    }
    if (!(g_ActiveActor->stateFlags & 0x80000)) {
        previousPhase = g_BattleSuspendedModeState;
        if (previousPhase == 6) {
            g_BattleSuspendedModeState = 0;
            g_BattleModeState = previousPhase;
        }
    }
    active = D_8009CD10->core;
    g_BattleStatusPanelSlot = 0;
    g_ActiveActor = active;
    g_MenuFrameCommand = Menu_RunFrameWithArg((int)&g_BattleMenuActionRecord);
    if (g_BattleModeState == 0 && g_BattleEnemyEscapeFlag != 0) {
        if (g_ActiveActor->hpAlive >= 0x2328U) {
            g_ActiveActor->hpAlive = 0x2328U;
            if (g_BattleGaugeCapLatch == 0) {
                srand(g_FrameRngCounter);
                if ((g_AkaoBgmHandle[0]) != 0) {
                    Akao_SendTableCommand((g_AkaoBgmHandle[0]), 0x454, 0, 0x80, 0x7F);
                }
                g_BattleGaugeCapLatch = 1;
            }
            if (!(g_ActiveActor->stateFlags & 0x2000) && (g_BattleLiveEnemyCount != 0)) {
                if ((g_FrameRngCounter & 3) == 0) {
                    register int topGreen asm("$8") = 0x46;
                    register int topBlue asm("$7") = 0x82;
                    register int bottomRed asm("$6") = 0x9F;
                    int bottomGreen = 0xFF;
                    int bottomBlue = 0xF9;
                    g_BattleAtbGauges[g_ActiveDrawSlot].r0 = 0;
                    g_BattleAtbGauges[g_ActiveDrawSlot].g0 = topGreen;
                    asm volatile("" : : "r"(bottomBlue));
                    g_BattleAtbGauges[g_ActiveDrawSlot].b0 = topBlue;
                    g_BattleAtbGauges[g_ActiveDrawSlot].r1 = bottomRed;
                    g_BattleAtbGauges[g_ActiveDrawSlot].g1 = bottomGreen;
                    g_BattleAtbGauges[g_ActiveDrawSlot].b1 = bottomBlue;
                    g_BattleAtbGauges[g_ActiveDrawSlot].r2 = 0;
                    g_BattleAtbGauges[g_ActiveDrawSlot].g2 = topGreen;
                    g_BattleAtbGauges[g_ActiveDrawSlot].b2 = topBlue;
                    g_BattleAtbGauges[g_ActiveDrawSlot].r3 = bottomRed;
                    g_BattleAtbGauges[g_ActiveDrawSlot].g3 = bottomGreen;
                    g_BattleAtbGauges[g_ActiveDrawSlot].b3 = bottomBlue;
                    g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.r = bottomRed;
                    g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.g = bottomGreen;
                    asm("" : : "i"(0));
                    g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.b = bottomBlue;
                } else if ((g_FrameRngCounter & 3) == 1) {
                    int red = 0x50;
                    register int green asm("$6") = 0xA3;
                    int blue = 0xBE;
                    g_BattleAtbGauges[g_ActiveDrawSlot].r0 = red;
                    g_BattleAtbGauges[g_ActiveDrawSlot].g0 = green;
                    asm volatile("" : : "r"(blue));
                    g_BattleAtbGauges[g_ActiveDrawSlot].b0 = blue;
                    g_BattleAtbGauges[g_ActiveDrawSlot].r1 = red;
                    g_BattleAtbGauges[g_ActiveDrawSlot].g1 = green;
                    g_BattleAtbGauges[g_ActiveDrawSlot].b1 = blue;
                    g_BattleAtbGauges[g_ActiveDrawSlot].r2 = red;
                    g_BattleAtbGauges[g_ActiveDrawSlot].g2 = green;
                    g_BattleAtbGauges[g_ActiveDrawSlot].b2 = blue;
                    g_BattleAtbGauges[g_ActiveDrawSlot].r3 = red;
                    g_BattleAtbGauges[g_ActiveDrawSlot].g3 = green;
                    g_BattleAtbGauges[g_ActiveDrawSlot].b3 = blue;
                    g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.r = red;
                    g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.g = green;
                    asm("" : : "i"(1));
                    g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.b = blue;
                } else if ((g_FrameRngCounter & 3) == 2) {
                    int topRed = 0x9F;
                    register int topGreen asm("$8") = 0xFF;
                    int topBlue = 0xF9;
                    int bottomGreen = 0x46;
                    int bottomBlue = 0x82;
                    g_BattleAtbGauges[g_ActiveDrawSlot].r0 = topRed;
                    g_BattleAtbGauges[g_ActiveDrawSlot].g0 = topGreen;
                    asm volatile("" : : "r"(bottomBlue));
                    g_BattleAtbGauges[g_ActiveDrawSlot].b0 = topBlue;
                    g_BattleAtbGauges[g_ActiveDrawSlot].r1 = 0;
                    g_BattleAtbGauges[g_ActiveDrawSlot].g1 = bottomGreen;
                    g_BattleAtbGauges[g_ActiveDrawSlot].b1 = bottomBlue;
                    g_BattleAtbGauges[g_ActiveDrawSlot].r2 = topRed;
                    g_BattleAtbGauges[g_ActiveDrawSlot].g2 = topGreen;
                    g_BattleAtbGauges[g_ActiveDrawSlot].b2 = topBlue;
                    g_BattleAtbGauges[g_ActiveDrawSlot].r3 = 0;
                    g_BattleAtbGauges[g_ActiveDrawSlot].g3 = bottomGreen;
                    g_BattleAtbGauges[g_ActiveDrawSlot].b3 = bottomBlue;
                    g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.r = 0;
                    g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.g = bottomGreen;
                    asm("" : : "i"(2));
                    g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.b = bottomBlue;
                } else if ((g_FrameRngCounter & 3) == 3) {
                    int red = 0x50;
                    register int green asm("$6") = 0xA3;
                    int blue = 0xBE;
                    g_BattleAtbGauges[g_ActiveDrawSlot].r0 = red;
                    g_BattleAtbGauges[g_ActiveDrawSlot].g0 = green;
                    asm volatile("" : : "r"(blue));
                    g_BattleAtbGauges[g_ActiveDrawSlot].b0 = blue;
                    g_BattleAtbGauges[g_ActiveDrawSlot].r1 = red;
                    g_BattleAtbGauges[g_ActiveDrawSlot].g1 = green;
                    g_BattleAtbGauges[g_ActiveDrawSlot].b1 = blue;
                    g_BattleAtbGauges[g_ActiveDrawSlot].r2 = red;
                    g_BattleAtbGauges[g_ActiveDrawSlot].g2 = green;
                    g_BattleAtbGauges[g_ActiveDrawSlot].b2 = blue;
                    g_BattleAtbGauges[g_ActiveDrawSlot].r3 = red;
                    g_BattleAtbGauges[g_ActiveDrawSlot].g3 = green;
                    g_BattleAtbGauges[g_ActiveDrawSlot].b3 = blue;
                    g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.r = red;
                    g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.g = green;
                    g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.b = blue;
                }
                if ((Pad_GetMenuPressedBitOrDisabled() << 0x18) == 0) {
                    opened = 0;
                    if (g_FieldPadPressed & 0x200) {
                        g_BattleItemMenuState = 0;
                        opened = 1;
                    } else if (g_FieldPadPressed & 0x80) {
                        opened = 1;
                        g_BattleItemMenuState = opened;
                        Inv_OpenAyaItemList(1);
                        Render_BeginSceneLoad();
                    }
                    if ((u8)opened != 0) {
                        Akao_Cmd_21(0, 0xFF);
                        g_BattleModeState = 1;
                        g_BattleAttackAnimFrame = 0;
                        g_GameStateFlags = g_GameStateFlags | 4;
                        menuMode = 0;
                        g_BattleTargetCount = Battle_BuildTargetList();
                        Battle_Init();
                        if (g_BattleTargetCount != 0) {
                            Menu_SetItemContext(0);
                        } else {
                            Menu_SetItemContext(1);
                        }
                    }
                }
            }
        }
        if ((Pad_GetMenuPressedBitOrDisabled() << 0x18) > 0) {
            if (!(g_ActiveActor->stateFlags & 0x4000) || (g_GameStateFlags & 0x100)) {
                menuMode = 2;
                Battle_StepScriptEntry();
            }
            if ((g_BattleHitPending) != 0) {
                Battle_ResolveHitOnTimer();
            }
        }
        if ((g_BattleResultFlags) & 0x300) {
            Save_DrawSlotMetadata();
        }
        Menu_MainUpdate(menuMode);
        entity = (BattleEntity *)g_FieldActorListHead;
        if (entity != 0) {
            do {
                if ((entity != D_8009CD10) && (entity->core != 0)) {
                    Battle_UpdateEnemy(entity);
                }
                entity = entity->next;
            } while (entity != 0);
        }
        if (g_BattleActionActorCount != 0) {
            Pm_SendCmd(g_BattleImpactPmSlot, 0, 1, (int)&(g_BattleActionActors[0]), 0, 0);
            Pm_SendCmd(g_BattleImpactPmSlot, 0, 0, 1, 0, 0);
            g_BattleActionActorCount = 0;
        }
        if (g_BattleSpritePrimCountdown != 0) {
            Draw_QueuePrimitive();
        }
        player = D_8009CD10;
        if ((player->animLastFrame == player->animPrev.parts.integer) && (player->actionMode < 4U)) {
            if (g_BattleHitActionPending == 0) {
                Entity_SetActionMode(player, g_ActiveActor->actionMode12);
                if (!(g_DiscChangeFlags & 1) && ((Pad_GetMenuPressedBitOrDisabled() << 0x18) == 0) && (g_BattleModeState == 0) && (g_ActiveActor->action->turnWord & 0x8000) && (g_BattlePendingEnemySpawn != 0)) {
                    Battle_RollEnemySpawn(g_BattlePendingEnemySpawn);
                    g_BattlePendingEnemySpawn = 0;
                }
            } else {
                if (g_BattleHitActionPending >= 2U) {
                    player->entityFlags |= 0x100;
                }
                g_BattleHitActionPending = 0;
                Entity_SetActionMode(D_8009CD10, g_BattleHitActionMode[0]);
                player = D_8009CD10;
                player->animFrame = g_BattleHitActionParam[0];
                player->animPrev.fixed = g_BattleHitActionParam[0] + 0xFFFF0000;
            }
        }
        if ((D_8009CD10->actionMode == 0xD) && !(g_DiscChangeFlags & 1)) {
            g_ActiveActor->actionMode12 = 4U;
            if (D_8009CD10->entityFlags & 0x100) {
                Asset_Find08Alt(0x453, 0, D_8009CD10->posX.parts.integer, D_8009CD10->posY.parts.integer, D_8009CD10->posZ.parts.integer);
                D_8009CD10->entityFlags = (D_8009CD10->entityFlags & ~0x100);
            }
            player = D_8009CD10;
            if (player->animLastFrame == player->animPrev.parts.integer) {
                Entity_SetActionMode(player, g_ActiveActor->actionMode12);
                D_8009CDA8 = D_8009CDA8 & ~1;
            }
        }
        g_BattleFrameCounter += 1;
    } else {
        phase = g_BattleModeState;
        if (phase == 1) {
            menuResult = Battle_HandleItemMenu();
            g_BattleModeState = menuResult;
            if (menuResult == 0) {
                g_GameStateFlags = g_GameStateFlags & ~4;
            }

        } else if (phase == 2) {
            Battle_StepLevelUp();

        } else if (phase == 3) {
            if (g_ActiveActor->stateFlags & 0x800) {
                if (Battle_StepVictory() & 0xFF) {
                    g_ActiveActor->stateFlags = (g_ActiveActor->stateFlags & ~0x800);
                }
            } else if (Inv_CountByValue(0x12) != 0) {
                if (Battle_StepVictory() & 0xFF) {
                    Inv_FindItemById(0x12);
                }
            } else {
                Battle_StepPostBattle();
            }
            status = g_ActiveActor;
            if (status->panelA_timer != 0) {
                Battle_DrawStatusPanel(0, (BattleStatusPanel *)&status->panelA_val);
                g_ActiveActor->panelA_timer--;
                status = g_ActiveActor;
            }
            if (status->panelAux_timer != 0) {
                Battle_DrawStatusPanel(0, (BattleStatusPanel *)&status->panelAux_val);
                g_ActiveActor->panelAux_timer = (u8) (g_ActiveActor->panelAux_timer - 1);
            }
            victoryEntity = (BattleEntity *)g_FieldActorListHead;
            if (victoryEntity != 0) {
                do {
                    if ((victoryEntity != D_8009CD10) && (victoryEntity->core != 0)) {
                        Battle_UpdateEnemy(victoryEntity);
                    }
                    victoryEntity = victoryEntity->next;
                } while (victoryEntity != 0);
            }

        } else if (phase == 4) {
            Battle_StepEscapeOrDeath();

        } else if (phase == 5) {
            Battle_StepEntityDeath();

        } else if (phase == 6) {
            Battle_PhaseInitEnemyTurn();

        } else if (phase == 7) {
            Battle_PhaseHitReaction();

        } else if (phase == 8) {
            Battle_PhaseEndTurn();

        }
    }
    if (((g_BattleSaveOverlayActive) != 0) && (g_BattleModeState == 0)) {
        Menu_SaveOverlayDraw();
    }
    if (g_BattleEnemyEscapeFlag != 0) {
        Battle_DrawActiveStatus();
    }
    if (g_MenuFrameCommand != 0) {
        Render_BeginSceneLoad();
    }
}

/* COMMON metadata retains retail GP-relative addressing; symbols provide storage. */
u8 g_BattleEndTimer, g_BattleStepIndex;
Combatant *g_ActiveActor;
int g_BattleModeState;
s8 g_BattleLiveEnemyCount;
/* Eight-stage transition: model fades, an effect spawn, stat recovery and
 * HUD reset. Returns 1 once the stats have been restored. */
int Battle_StepVictory(void)
{
    int result = 0;
    BattleEntity *player;
    BattleEntity *spawnPlayer;
    Combatant *core;
    u8 countdown;
    u16 brightness;
    Combatant *restoredCore;
    switch (g_BattleStepIndex) {
    case 0:
        player = D_8009CD10;
        if (player->actionMode != 0x13) {
            Entity_SetActionMode(player, 0x13);
            player = D_8009CD10;
        }
        if (player->animLastFrame != (u16)(player->animFrame >> 16)) {
            player->entityFlags &= ~0x100;
            break;
        }
        g_BattleEndTimer = 16;
        player->entityFlags |= 0x100;
        g_BattleStepIndex++;
        break;
    case 1:
        brightness = g_BattleEndTimer;
        if (brightness != 0) {
            brightness = ~(brightness << 3) & 0xFF;
            Render_FadeEntityColor(&D_8009CD10->renderObject, brightness, brightness, brightness);
            Render_FadeEntityColor(&g_PlayerBattleEntity, brightness, brightness, brightness);
            g_BattleEndTimer--;
            break;
        }
        Render_FadeEntityColor(&D_8009CD10->renderObject, 255, 255, 255);
        {
            RenderObjectEntity *partner = &g_PlayerBattleEntity;

            Render_FadeEntityColor(partner, 255, 255, 255);
            Anim_SetInterpRate(&D_8009CD10->renderObject, 30);
            D_8009CD10->renderObject.flags_9C |= 2;
            Anim_SetInterpRate(partner, 30);
            g_PlayerBattleEntity.flags_9C |= 2;
        }
        g_BattleStepIndex++;
        break;
    case 2:
        if (D_8009CD10->renderObject.variant_visible != 0 || g_PlayerBattleEntity.variant_visible != 0)
            break;
        Scene_LoadRoomAssets(0x69, D_8009CD10);
        spawnPlayer = D_8009CD10;
        Asset_Find08Alt(0x4AF, 0, spawnPlayer->posX.parts.integer, spawnPlayer->posY.parts.integer, spawnPlayer->posZ.parts.integer);
        g_BattleEndTimer = 0x34;
        g_BattleStepIndex++;
        break;
    case 3:
        if (g_BattleEndTimer != 0) {
            g_BattleEndTimer--;
            break;
        }
        D_8009CD10->renderObject.variant_visible = 1;
        Anim_SetInterpRate(&D_8009CD10->renderObject, 30);
        D_8009CD10->renderObject.flags_9C |= 4;
        Render_FadeEntityColor(&D_8009CD10->renderObject, 255, 255, 255);
        g_PlayerBattleEntity.variant_visible = 1;
        {
            RenderObjectEntity *partner = &g_PlayerBattleEntity;

            Anim_SetInterpRate(partner, 30);
            g_PlayerBattleEntity.flags_9C |= 4;
            Render_FadeEntityColor(partner, 255, 255, 255);
        }
        Entity_SetActionMode(D_8009CD10, 0xF);
        g_BattleEndTimer = 30;
        D_8009CD10->animFrame = 0x230000;
        D_8009CD10->animPrev.fixed = 0x220000;
        g_BattleStepIndex++;
        break;
    case 4:
        countdown = g_BattleEndTimer;
        if (countdown == 16) {
            g_BattleStepIndex++;
            player = D_8009CD10;
            player->entityFlags &= ~0x100;
        } else {
            g_BattleEndTimer = countdown - 1;
        }
        break;
    case 5:
        brightness = g_BattleEndTimer;
        if (brightness != 0) {
            brightness = ((brightness << 3) + 127) & 0xFF;
            Render_FadeEntityColor(&D_8009CD10->renderObject, brightness, brightness, brightness);
            Render_FadeEntityColor(&g_PlayerBattleEntity, brightness, brightness, brightness);
            g_BattleEndTimer--;
        } else {
            g_BattleStepIndex++;
            D_8009CD10->renderObject.flags_9C |= 0x20;
        }
        break;
    case 6:
        player = D_8009CD10;
        if (player->animLastFrame != player->animPrev.parts.integer)
            break;
        Entity_SetActionMode(player, g_ActiveActor->actionMode12);
        g_BattleStepIndex++;
        break;
    case 7:
        core = g_ActiveActor;
        core->curHP = (s16)core->maxHP >> 1;
        core->exp_or_acc >>= 2;
        Battle_ResetEnemyStats(1);
        restoredCore = g_ActiveActor;
        player = D_8009CD10;
        restoredCore->panelB_val = restoredCore->curHP;
        restoredCore->panelB_x = player->renderObject.projected_x;
        restoredCore->panelB_y = player->renderObject.projected_y;
        restoredCore->panelB_timer = 30;
        g_ActiveActor->panelB_flag = 1;
        Aya_DeriveStats(&g_ActiveActor->atbStep, &g_ActiveActor->atbRate);
        BATTLE_SET_GAUGE_GRADIENT(g_BattleAtbGauges[0], 0, 0x46, 0x82, 0x9F, 0xFF, 0xF9);
        BATTLE_SET_SPRITE_COLOR(g_BattleAtbMarkers[0].sprite, 0x9F, 0xFF, 0xF9);
        BATTLE_SET_GAUGE_GRADIENT(g_BattleAtbGauges[1], 0, 0x46, 0x82, 0x9F, 0xFF, 0xF9);
        BATTLE_SET_SPRITE_COLOR(g_BattleAtbMarkers[1].sprite, 0x9F, 0xFF, 0xF9);
        BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
        BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
        g_BattleStepIndex = 0;
        g_BattleModeState = 0;
        if (g_BattleLiveEnemyCount == 0)
            Battle_StartDeathAnim();
        result = 1;
        break;
    }
    return result;
}

void Battle_StepLevelUp(void) {
    u8 step;
    BattleEntity *entity;
    u32 flags;

    step = g_BattleStepIndex;
    switch (step) {
    case 0:
        entity = D_8009CD10;
        if ((u16)(entity->animFrame >> 16) != 0xA && (g_GameStateFlags & 0x800) == 0) {
            break;
        }
        {
            u32 entity_flags;
            entity_flags = entity->entityFlags;
            entity->entityFlags = entity_flags | 0x100;
        }
        Pm_StopAllBoth();
        Aya_SetTotalExp(g_BattleExpReward, g_BattlePeBonusReward, g_BattleRewardSlots);
        Render_BeginSceneLoad();
        g_BattleStepIndex++;
        break;

    case 1:
        if (g_MenuFrameCommand == 0x3E8) {
            entity = D_8009CD10;
            g_BattleStepIndex = step + 1;
            entity->entityFlags &= -0x101;
        }
        break;

    case 2:
        entity = D_8009CD10;
        if (entity->animLastFrame == entity->animPrev.parts.integer) {
            flags = g_GameStateFlags;
            if ((flags & 0x1800) == 0) {
                Entity_SetActionMode(entity, 0x15);
            } else {
                u32 cleared_flags;
                cleared_flags = flags & -0x1801;
                g_GameStateFlags = cleared_flags;
                Entity_SetActionMode(entity, 0x18);
            }
            g_BattleStepIndex++;
        }
        break;

    case 3:
        if (CdRom_StepReadState(0) != 1) {
            Battle_SetupPlayerPalette();
            g_BattleModeState = 9;
            g_GameState.flags &= 0xFFFF7FFF;
        }
        break;
    }
}

/* COMMON metadata preserves GP-relative accesses; retail symbols supply storage. */
u8 g_BattleEndTimer, g_BattleStepIndex;
s32 g_BattleModeState;

/* End-of-battle sequence: retire enemy actors, fade sound and animate the
 * textured panel before completing the battle phase. Matching debt: register
 * pins, empty barriers, a volatile packet view and 24 reserved stack bytes
 * in the other cases; case 3 sets the corners through a quad pointer.
 * Countdown and stage stores remain local to their switch cases. */
void Battle_StepPostBattle(void)
{
    volatile u8 matchingStackReserve[24];
    BattleEntity *player;
    BattleEntity *entity;
    RenderTexturedQuad *expandingPanel;
    volatile RenderTexturedQuad *fadingPanel;
    s32 remainingFrames;
    register s32 horizontalInset asm("$7");
    s8 brightness;
    register u16 horizontalEdge asm("$4");
    u16 rightEdge;
    u32 entityFlags;
    u8 fadeBrightness;
    u8 countdown;
    EnemyCombatant *core;
    EnemyCombatant *actionCore;
    EnemyCombatant *fadingCore;

    switch (g_BattleStepIndex) {
    case 0:
        player = D_8009CD10;
        if (player->actionMode != 0x13) {
            Entity_SetActionMode(player, 0x13);
            player = D_8009CD10;
        }
        if (player->animLastFrame == (u16)(player->animFrame >> 16)) {
            entity = (BattleEntity *)g_FieldActorListHead;
            if (entity != 0) {
                do {
                        if (entity != D_8009CD10) {
                            core = entity->core;
                            if ((core->field04.bytes.kind != 1) && ((core != 0) || !(entity->entityFlags & 0x40))) {
                                entity->renderObject.flags_9C |= 2;
                                actionCore = entity->core;
                                entity->entityFlags |= 0x1000;
                                if (actionCore != 0) {
                                    Entity_SetActionMode(entity, (u16)(s8)actionCore->lowAndEntityId.bytes.low);
                                    entityFlags = entity->entityFlags;
                                    if ((entityFlags & 0x40000000) && (((EnemyCombatant *)entity->core)->deathPersist == 0)) {
                                        entity->entityFlags = entityFlags | 0x10;
                                        entity->core = 0;
                                    }
                                }
                                entity->motionX = 0;
                                entity->motionY = 0;
                                entity->motionZ = 0;
                            }
                        }
                        entity = entity->next;
                    } while (entity != 0);
                }
                Battle_ResetEnemyStats(0);
                g_BattleEndTimer = 0x46;
                g_BattleStepIndex += 1;
                D_8009CD10->entityFlags |= 0x100;
            }
            else {
                player->entityFlags &= ~0x100;
            }
            break;
        case 1: {
            s32 exemptKind;
            if (g_BattleEndTimer == 0x3C) {
                Render_StartFadeIn(0x3C);
                Akao_Cmd_C1_WithSlot(0, 0x3C, 0);
                exemptKind = 1;
                entity = (BattleEntity *)g_FieldActorListHead;
                if (entity != 0) {
                    do {
                    if (entity != D_8009CD10) {
                        fadingCore = entity->core;
                        if ((fadingCore->field04.bytes.kind != exemptKind) && ((fadingCore != 0) || !(entity->entityFlags & 0x40))) {
                            Anim_SetInterpRate(&entity->renderObject, 0x3C);
                        }
                    }
                    entity = entity->next;
                } while (entity != 0);
            }
        }
        else {
            entity = (BattleEntity *)g_FieldActorListHead;
            if (entity != 0) {
                do {
                    if ((entity != D_8009CD10) && ((entity->core != 0) || !(entity->entityFlags & 0x40)) && (entity->renderObject.variant_visible == 0)) {
                        entity->entityFlags |= 0x10;
                    }
                    entity = entity->next;
                } while (entity != 0);
            }
        }
        countdown = g_BattleEndTimer;
        if (countdown == 0) {
            Pm_StopAllBoth();
            Akao_Cmd_21(0, 0xFF);
            g_BattleEndTimer = 0x1E;
            g_BattleStepIndex++;
            break;
        }
        else {
            g_BattleEndTimer--;
        }
        break;
    }
    case 2: {
        register u16 top asm("$5");
        u8 timer;
        register RenderTexturedQuad *base asm("$3");
        s32 frame;
        brightness = 120;
        top = 122;
        asm volatile("" : : "r"(top));
        brightness -= g_BattleEndTimer * 4;
        g_BattleEndPanels[g_ActiveDrawSlot].color.bytes.r = brightness;
        g_BattleEndPanels[g_ActiveDrawSlot].color.bytes.g = brightness;
        g_BattleEndPanels[g_ActiveDrawSlot].color.bytes.b = brightness;
        base = g_BattleEndPanels;
        frame = g_ActiveDrawSlot;
        expandingPanel = &base[frame];
        timer = g_BattleEndTimer;
        asm volatile("" : "=r"(expandingPanel) : "0"(expandingPanel));
        remainingFrames = timer & 0xFF;
        horizontalInset = remainingFrames * 4;
        horizontalEdge = 0x64 - horizontalInset;
        rightEdge = ((remainingFrames * 8) + 0xDC) - horizontalInset;
        expandingPanel->x0 = horizontalEdge;
        expandingPanel->x2 = horizontalEdge;
        expandingPanel->y0 = top;
        expandingPanel->x1 = rightEdge;
        expandingPanel->y1 = top;
        expandingPanel->y2 = 0x7C;
        expandingPanel->x3 = rightEdge;
        expandingPanel->y3 = 0x7C;
        if (remainingFrames != 0) {
            {
                u8 nextTimer = timer - 1;
                g_BattleEndTimer = nextTimer;
            }
        }
        else {
            g_BattleEndTimer = 0x50;
            g_BattleStepIndex++;
            break;
        }
        break;
    }
    case 3: {
        if (g_BattleEndTimer >= 0x1AU) {
            u8 pulse = rcos((g_BattleEndTimer - 16) * 16) * 11U >> 11;
            RenderTexturedQuad *panels = g_BattleEndPanels;
            RenderTexturedQuad *panel = &panels[g_ActiveDrawSlot];
            BATTLE_SET_QUAD_XY(*panel, 100, 122 - pulse, 220, 122 - pulse, 100, 124, 220, 124);
        }
        else {
            RenderTexturedQuad *panels = g_BattleEndPanels;
            RenderTexturedQuad *panel = &panels[g_ActiveDrawSlot];
            BATTLE_SET_QUAD_XY(*panel, 100, 100, 220, 100, 100, 124, 220, 124);
        }
        countdown = g_BattleEndTimer;
        if (countdown == 0) {
            Anim_SetInterpRate(&D_8009CD10->renderObject, 0x3C);
            D_8009CD10->renderObject.flags_9C |= 2;
            Anim_SetInterpRate(&g_PlayerBattleEntity, 0x3C);
            {
                u8 state = g_BattleStepIndex;
                g_BattleEndTimer = 0x3C;
                state++;
                g_PlayerBattleEntity.flags_9C |= 2;
                g_BattleStepIndex = state;
            }
        }
        else {
            g_BattleEndTimer--;
        }
        break;
    }
    case 4: {
        register RenderTexturedQuad *base asm("$7");
        register u16 right asm("$6");
        if ((D_8009CD10->renderObject.variant_visible == 0) && (g_PlayerBattleEntity.variant_visible == 0)) {
            g_BattleStepIndex++;
            break;
        }
        base = g_BattleEndPanels;
        right = 220;
        fadeBrightness = g_BattleEndTimer * 2;
        fadingPanel = &base[g_ActiveDrawSlot];
        fadingPanel->color.bytes.r = fadeBrightness;
        fadingPanel->x0 = 0x64;
        fadingPanel->y0 = 0x64;
        fadingPanel->y1 = 0x64;
        fadingPanel->x2 = 0x64;
        fadingPanel->x1 = right;
        fadingPanel->y2 = 0x7C;
        fadingPanel->x3 = right;
        fadingPanel->y3 = 0x7C;
        base[g_ActiveDrawSlot].color.bytes.g = fadeBrightness;
        base[g_ActiveDrawSlot].color.bytes.b = fadeBrightness;
        g_BattleEndTimer--;
        break;
    }
    case 5:
        Battle_SetupPlayerPalette();
        g_BattleModeState = -1;
        Boot_Shutdown();
        break;
    }
    AddPrim((u32 *)(D_800B08F8.ordering[g_ActiveDrawSlot] + 4),
    (u32 *)&g_BattleEndPanels[g_ActiveDrawSlot]);
}

void Battle_StepEscapeOrDeath(void)
{
    BattleEntity *entity;

    switch (g_BattleStepIndex) {
    case 0:
        g_BattleEnemyEscapeFlag = 0;
        Pm_StopUpperHalf();
        for (entity = (BattleEntity *)g_FieldActorListHead; entity != 0; entity = entity->next) {
            if (entity->core == 0) {
                if (entity->parent == 0) continue;
            } else if (entity == D_8009CD10) {
                Anim_SetInterpRate(&g_PlayerBattleEntity, 30);
                D_800B0D88_write.value = D_800B0D88_read.value | 2;
                D_8009CD10->renderObject.flags_9C |= 2;
            } else {
                u32 flags;
                Entity_SetActionMode(entity,
                    (u16)(s8)((EnemyCombatant *)entity->core)->lowAndEntityId.bytes.low);
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
        g_BattleStepIndex++;
        break;
    case 1:
        if (D_8009CD10->renderObject.variant_visible != 0 ||
            g_PlayerBattleEntity.variant_visible != 0) break;
        Battle_ResetEnemyStats(0);
        g_BattleStepIndex++;
        entity = (BattleEntity *)g_FieldActorListHead;
        if (entity != 0) {
            BattleEntity *player = D_8009CD10;
            do {
                if (entity == player) continue;
                if (entity->core != 0 || entity->parent != 0)
                    entity->renderObject.flags_9C |= 2;
            } while ((entity = entity->next) != 0);
        }
        break;
    case 2: {
        int all_done = 1;
        entity = (BattleEntity *)g_FieldActorListHead;
        if (entity != 0) {
            BattleEntity *player = D_8009CD10;
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
            g_BattleStepIndex++;
        }
        break;
    }
    case 3:
        if (CdRom_StepReadState(0) != 1) {
            Battle_ClearMotionTable();
            Entity_SetActionMode(D_8009CD10, 0x15);
            Battle_SetupPlayerPalette();
            g_BattleModeState = 10;
        }
        break;
    }
}

/* Initialize the enemy turn and wait for HUD, animation and texture work. */
void Battle_PhaseInitEnemyTurn(void) {
    /* Retail reserves this unused local area; its original purpose is unknown. */
    volatile u8 matchingStackReserve[0x300];
    s32 entityFlags;
    u8 ready;
    Combatant *actor;
    EnemyCombatant *enemy;
    BattleEntity *entity;
    BattleEntity *player;
    Combatant *active;
    BattleEntity *initialPlayer;
    register s32 moveLock asm("$3");

    u32 hp;
    s32 gameFlags;
    s32 clearMask;
    /* Clear prior script effects and stop the player before waiting for actors. */
    Battle_FlushScriptSounds();
    ready = 1;
    Draw_ResetTextboxes();
    moveLock = D_8009CDA8 | 1;
    D_8009CDA8 = moveLock;
    initialPlayer = D_8009CD10;
    active = g_ActiveActor;
    clearMask = ~4;
    initialPlayer->motionX = 0;
    initialPlayer->motionY = 0;
    initialPlayer->motionZ = 0;
    gameFlags = g_GameStateFlags;
    hp = active->hpAlive;
    g_GameStateFlags = gameFlags & clearMask;
    /* Animate the charged gauge and status highlight using the draw slot. */
    if (hp >= 0x2328U) {
        if ((g_FrameRngCounter & 3) == 0) {
            register int topGreen asm("$8") = 0x46;
            register int topBlue asm("$7") = 0x82;
            register int bottomRed asm("$6") = 0x9F;
            int bottomGreen = 0xFF;
            int bottomBlue = 0xF9;
            g_BattleAtbGauges[g_ActiveDrawSlot].r0 = 0;
            g_BattleAtbGauges[g_ActiveDrawSlot].g0 = topGreen;
            asm volatile("" : : "r"(bottomBlue));
            g_BattleAtbGauges[g_ActiveDrawSlot].b0 = topBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r1 = bottomRed;
            g_BattleAtbGauges[g_ActiveDrawSlot].g1 = bottomGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b1 = bottomBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r2 = 0;
            g_BattleAtbGauges[g_ActiveDrawSlot].g2 = topGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b2 = topBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r3 = bottomRed;
            g_BattleAtbGauges[g_ActiveDrawSlot].g3 = bottomGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b3 = bottomBlue;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.r = bottomRed;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.g = bottomGreen;
            asm("" : : "i"(0));
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.b = bottomBlue;
        } else if ((g_FrameRngCounter & 3) == 1) {
            int red = 0x50;
            register int green asm("$6") = 0xA3;
            int blue = 0xBE;
            g_BattleAtbGauges[g_ActiveDrawSlot].r0 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g0 = green;
            asm volatile("" : : "r"(blue));
            g_BattleAtbGauges[g_ActiveDrawSlot].b0 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r1 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g1 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b1 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r2 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g2 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b2 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r3 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g3 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b3 = blue;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.r = red;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.g = green;
            asm("" : : "i"(1));
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.b = blue;
        } else if ((g_FrameRngCounter & 3) == 2) {
            register int topRed asm("$7") = 0x9F;
            register int topGreen asm("$8") = 0xFF;
            register int topBlue asm("$6") = 0xF9;
            int bottomGreen = 0x46;
            int bottomBlue = 0x82;
            g_BattleAtbGauges[g_ActiveDrawSlot].r0 = topRed;
            g_BattleAtbGauges[g_ActiveDrawSlot].g0 = topGreen;
            asm volatile("" : : "r"(bottomBlue));
            g_BattleAtbGauges[g_ActiveDrawSlot].b0 = topBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r1 = 0;
            g_BattleAtbGauges[g_ActiveDrawSlot].g1 = bottomGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b1 = bottomBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r2 = topRed;
            g_BattleAtbGauges[g_ActiveDrawSlot].g2 = topGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b2 = topBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r3 = 0;
            g_BattleAtbGauges[g_ActiveDrawSlot].g3 = bottomGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b3 = bottomBlue;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.r = 0;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.g = bottomGreen;
            asm("" : : "i"(2));
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.b = bottomBlue;
        } else if ((g_FrameRngCounter & 3) == 3) {
            int red = 0x50;
            int green = 0xA3;
            int blue = 0xBE;
            g_BattleAtbGauges[g_ActiveDrawSlot].r0 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g0 = green;
            asm volatile("" : : "r"(red), "r"(blue));
            g_BattleAtbGauges[g_ActiveDrawSlot].b0 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r1 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g1 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b1 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r2 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g2 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b2 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r3 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g3 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b3 = blue;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.r = red;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.g = green;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.b = blue;
        }
    }
    if (g_ActiveActor->stateFlags & 0x2000) {
        if ((g_FrameRngCounter & 3) == 0) {
            register int topRed asm("$9") = 0xFF;
            register int topGreen asm("$8") = 0x3D;
            register int topBlue asm("$7") = 0x81;
            register int bottomRed asm("$6") = 0x83;
            int bottomGreen = 0x13;
            int bottomBlue = 1;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r0 = topRed;
            asm volatile("" : : "r"(bottomBlue));
            g_BattlePeGauges[g_ActiveDrawSlot][1].g0 = topGreen;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b0 = topBlue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r1 = bottomRed;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g1 = bottomGreen;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b1 = bottomBlue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r2 = topRed;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g2 = topGreen;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b2 = topBlue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r3 = bottomRed;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g3 = bottomGreen;
            asm("" : : "i"(0));
            g_BattlePeGauges[g_ActiveDrawSlot][1].b3 = bottomBlue;
        } else if ((g_FrameRngCounter & 3) == 1) {
            register int red asm("$6") = 0xC1;
            int green = 0x28;
            int blue = 0x41;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r0 = red;
            asm volatile("" : : "r"(blue));
            g_BattlePeGauges[g_ActiveDrawSlot][1].g0 = green;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b0 = blue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r1 = red;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g1 = green;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b1 = blue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r2 = red;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g2 = green;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b2 = blue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r3 = red;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g3 = green;
            asm("" : : "i"(1));
            g_BattlePeGauges[g_ActiveDrawSlot][1].b3 = blue;
        } else if ((g_FrameRngCounter & 3) == 2) {
            register int topRed asm("$9") = 0x83;
            register int topGreen asm("$8") = 0x13;
            register int topBlue asm("$7") = 1;
            register int bottomRed asm("$6") = 0xFF;
            int bottomGreen = 0x3D;
            int bottomBlue = 0x81;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r0 = topRed;
            asm volatile("" : : "r"(bottomBlue));
            g_BattlePeGauges[g_ActiveDrawSlot][1].g0 = topGreen;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b0 = topBlue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r1 = bottomRed;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g1 = bottomGreen;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b1 = bottomBlue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r2 = topRed;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g2 = topGreen;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b2 = topBlue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r3 = bottomRed;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g3 = bottomGreen;
            asm("" : : "i"(2));
            g_BattlePeGauges[g_ActiveDrawSlot][1].b3 = bottomBlue;
        } else if ((g_FrameRngCounter & 3) == 3) {
            register int red asm("$6") = 0xC1;
            int green = 0x28;
            int blue = 0x41;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r0 = red;
            asm volatile("" : : "r"(blue));
            g_BattlePeGauges[g_ActiveDrawSlot][1].g0 = green;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b0 = blue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r1 = red;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g1 = green;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b1 = blue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r2 = red;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g2 = green;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b2 = blue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r3 = red;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g3 = green;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b3 = blue;
        }
    }
    /* Floating status panels must finish before the next phase. */
    actor = g_ActiveActor;
    if (actor->panelA_timer != 0) {
        Battle_DrawStatusPanel(0, (BattleStatusPanel *)&actor->panelA_val);
        ready = 0;
        g_ActiveActor->panelA_timer = (u8) (g_ActiveActor->panelA_timer - 1);
    }
    actor = g_ActiveActor;
    if (actor->panelB_timer != 0) {
        Battle_DrawStatusPanel(0, (BattleStatusPanel *)&actor->panelB_val);
        ready = 0;
        g_ActiveActor->panelB_timer = (u8) (g_ActiveActor->panelB_timer - 1);
    }
    if (g_ActiveActor->panelAux_timer != 0) {
        Battle_DrawStatusPanel(0, (BattleStatusPanel *)&g_ActiveActor->panelAux_val);
        ready = 0;
        g_ActiveActor->panelAux_timer = (u8) (g_ActiveActor->panelAux_timer - 1);
    }
    /* Wait for player/enemy animations to reach their requested idle modes. */
    entity = (BattleEntity *)g_FieldActorListHead;
    if (entity != 0) {
        do {
            if (entity == D_8009CD10) {
                if (entity->actionMode != g_ActiveActor->actionMode12) {
                    ready = 0;
                    entityFlags = entity->entityFlags & ~0x100;
                    entity->entityFlags = entityFlags;
                    if (entity->animLastFrame == entity->animPrev.parts.integer) {
                        if ((entity->actionMode < 4U) && (g_BattleHitActionPending != 0)) {
                            if (g_BattleHitActionPending >= 2U) {
                                entity->entityFlags = (entityFlags | 0x100);
                            }
                            g_BattleHitActionPending = 0;
                            Entity_SetActionMode(D_8009CD10, g_BattleHitActionMode[0]);
                            player = D_8009CD10;
                            player->animFrame = g_BattleHitActionParam[0];
                            player->animPrev.fixed = (g_BattleHitActionParam[0] + 0xFFFF0000);
                        } else {
                            Entity_SetActionMode(entity, g_ActiveActor->actionMode12);
                        }
                    }
                }
            } else {
                enemy = entity->core;
                if ((enemy != 0) && (enemy->field04.bytes.kind != 1)) {
                    if ((entity->actionMode - 2) >= 2U) {
                        entity->entityFlags = (entity->entityFlags & ~0x1000);
                        Entity_TickAnimSequences(entity);
                        ready = 0;
                        if (entity->animLastFrame == entity->animPrev.parts.integer) {
                            Entity_SetActionMode(entity, (u16)(s8)((EnemyCombatant *)entity->core)->lowAndEntityId.bytes.low);
                        } else if (entity->animStep != 0x10000) {
                            entity->animStep = 0x10000;
                        }
                    }
                    Battle_UpdateEnemy(entity);
                }
            }
            entity = entity->next;
        } while (entity != 0);
    }
    /* Texture uploads can keep the phase pending after animations settle. */
    if (Asset_LoadTimTextures(0) != 0) {
        ready = 0;
    }
    if (ready != 0) {
        Akao_Cmd_21(0, 0xFF);
        {
            Combatant *finisher = g_ActiveActor;
            finisher->knockbackDistance = 0;
            finisher->knockbackFrames = 0;
        }
        g_BattleModeState = 7;
        D_8009CDA8 &= ~1;
        BATTLE_SET_GAUGE_GRADIENT(g_BattleAtbGauges[0], 0, 0x46, 0x82, 0x9F, 0xFF, 0xF9);
        BATTLE_SET_SPRITE_COLOR(g_BattleAtbMarkers[0].sprite, 0x9F, 0xFF, 0xF9);
        BATTLE_SET_GAUGE_GRADIENT(g_BattleAtbGauges[1], 0, 0x46, 0x82, 0x9F, 0xFF, 0xF9);
        BATTLE_SET_SPRITE_COLOR(g_BattleAtbMarkers[1].sprite, 0x9F, 0xFF, 0xF9);
        BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
        BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
        BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][1], 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
        BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][1], 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
    }
}

void Battle_PhaseHitReaction(void) {
    volatile u8 matchingStackReserve[0x1A0];
    Combatant *active;
    BattleEntity *player;
    BattleEntity *entity;
    EnemyCombatant *enemy;
    if (g_ActiveActor->hpAlive >= 0x2328U) {
        if ((g_FrameRngCounter & 3) == 0) {
            register int topGreen asm("$8") = 0x46;
            register int topBlue asm("$7") = 0x82;
            register int bottomRed asm("$6") = 0x9F;
            int bottomGreen = 0xFF;
            int bottomBlue = 0xF9;
            g_BattleAtbGauges[g_ActiveDrawSlot].r0 = 0;
            g_BattleAtbGauges[g_ActiveDrawSlot].g0 = topGreen;
            asm volatile("" : : "r"(bottomBlue));
            g_BattleAtbGauges[g_ActiveDrawSlot].b0 = topBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r1 = bottomRed;
            g_BattleAtbGauges[g_ActiveDrawSlot].g1 = bottomGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b1 = bottomBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r2 = 0;
            g_BattleAtbGauges[g_ActiveDrawSlot].g2 = topGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b2 = topBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r3 = bottomRed;
            g_BattleAtbGauges[g_ActiveDrawSlot].g3 = bottomGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b3 = bottomBlue;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.r = bottomRed;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.g = bottomGreen;
            asm("" : : "i"(0));
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.b = bottomBlue;
        } else if ((g_FrameRngCounter & 3) == 1) {
            int red = 0x50;
            register int green asm("$6") = 0xA3;
            int blue = 0xBE;
            g_BattleAtbGauges[g_ActiveDrawSlot].r0 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g0 = green;
            asm volatile("" : : "r"(blue));
            g_BattleAtbGauges[g_ActiveDrawSlot].b0 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r1 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g1 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b1 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r2 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g2 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b2 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r3 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g3 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b3 = blue;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.r = red;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.g = green;
            asm("" : : "i"(1));
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.b = blue;
        } else if ((g_FrameRngCounter & 3) == 2) {
            register int topRed asm("$7") = 0x9F;
            register int topGreen asm("$8") = 0xFF;
            register int topBlue asm("$6") = 0xF9;
            int bottomGreen = 0x46;
            int bottomBlue = 0x82;
            g_BattleAtbGauges[g_ActiveDrawSlot].r0 = topRed;
            g_BattleAtbGauges[g_ActiveDrawSlot].g0 = topGreen;
            asm volatile("" : : "r"(bottomBlue));
            g_BattleAtbGauges[g_ActiveDrawSlot].b0 = topBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r1 = 0;
            g_BattleAtbGauges[g_ActiveDrawSlot].g1 = bottomGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b1 = bottomBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r2 = topRed;
            g_BattleAtbGauges[g_ActiveDrawSlot].g2 = topGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b2 = topBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r3 = 0;
            g_BattleAtbGauges[g_ActiveDrawSlot].g3 = bottomGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b3 = bottomBlue;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.r = 0;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.g = bottomGreen;
            asm("" : : "i"(2));
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.b = bottomBlue;
        } else if ((g_FrameRngCounter & 3) == 3) {
            int red = 0x50;
            int green = 0xA3;
            int blue = 0xBE;
            g_BattleAtbGauges[g_ActiveDrawSlot].r0 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g0 = green;
            asm volatile("" : : "r"(red), "r"(blue));
            g_BattleAtbGauges[g_ActiveDrawSlot].b0 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r1 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g1 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b1 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r2 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g2 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b2 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r3 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g3 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b3 = blue;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.r = red;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.g = green;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.b = blue;
        }
    }
    active = g_ActiveActor;
    if (active->panelA_timer) {
        if (active->panelA_timer == 30) {
            BattleEntity *panelPlayer = D_8009CD10;
            active->panelA_x = panelPlayer->renderObject.projected_x;
            active->panelA_y = panelPlayer->renderObject.projected_y;
        }
        Battle_DrawStatusPanel(0, (BattleStatusPanel *)&g_ActiveActor->panelA_val);
        g_ActiveActor->panelA_timer--;
    }
    active = g_ActiveActor;
    if (active->panelB_timer) {
        if (active->panelB_timer == 30) {
            player = D_8009CD10;
            active->panelB_x = player->renderObject.projected_x;
            active->panelB_y = player->renderObject.projected_y;
            active->panelB_flag = 1;
        }
        Battle_DrawStatusPanel(0, (BattleStatusPanel *)&g_ActiveActor->panelB_val);
        g_ActiveActor->panelB_timer--;
    }
    for (entity = (BattleEntity *)g_FieldActorListHead; entity; entity = entity->next) {
        if (entity != D_8009CD10 && entity->core) {
            enemy = entity->core;
            if (enemy->coreFlags & 0x6000) {
                Battle_ProcessActionSlot(entity);
                if ((enemy->coreFlags & 0x6000) == 0x2000)
                    enemy->coreFlags = (enemy->coreFlags & ~0x6000) | 0x4000;
            }
            if (enemy->panelC_timer) {
                if (enemy->panelC_timer == 30) {
                    enemy->panelC_x = entity->renderObject.projected_x;
                    enemy->panelC_y = entity->renderObject.projected_y;
                }
                Battle_DrawStatusPanel(1, (BattleStatusPanel *)&enemy->panelC_val);
                enemy->panelC_timer--;
            }
        }
    }
}

void Battle_PhaseEndTurn(void) {
    /* Retail reserves 0x300 unused bytes; original local layout is unknown. */
    volatile u8 matchingStackReserve[0x300];
    BattleEntity *player;
    Combatant *active;
    BattleEntity *initialPlayer;
    BattleEntity *entity;
    EnemyCombatant *enemy;
    u8 ready;
    u8 mode;
    u8 step;
    u8 nextMode;
    u32 hp;

    initialPlayer = D_8009CD10;
    active = g_ActiveActor;
    initialPlayer->motionX = 0;
    initialPlayer->motionY = 0;
    initialPlayer->motionZ = 0;
    hp = active->hpAlive;
    ready = 1;
    /* Animate the charged gauge and active status colors. */
    if (hp >= 0x2328U) {
        if ((g_FrameRngCounter & 3) == 0) {
            register int topGreen asm("$8") = 0x46;
            register int topBlue asm("$7") = 0x82;
            register int bottomRed asm("$6") = 0x9F;
            int bottomGreen = 0xFF;
            int bottomBlue = 0xF9;
            g_BattleAtbGauges[g_ActiveDrawSlot].r0 = 0;
            g_BattleAtbGauges[g_ActiveDrawSlot].g0 = topGreen;
            asm volatile("" : : "r"(bottomBlue));
            g_BattleAtbGauges[g_ActiveDrawSlot].b0 = topBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r1 = bottomRed;
            g_BattleAtbGauges[g_ActiveDrawSlot].g1 = bottomGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b1 = bottomBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r2 = 0;
            g_BattleAtbGauges[g_ActiveDrawSlot].g2 = topGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b2 = topBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r3 = bottomRed;
            g_BattleAtbGauges[g_ActiveDrawSlot].g3 = bottomGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b3 = bottomBlue;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.r = bottomRed;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.g = bottomGreen;
            asm("" : : "i"(0));
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.b = bottomBlue;
        } else if ((g_FrameRngCounter & 3) == 1) {
            int red = 0x50;
            register int green asm("$6") = 0xA3;
            int blue = 0xBE;
            g_BattleAtbGauges[g_ActiveDrawSlot].r0 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g0 = green;
            asm volatile("" : : "r"(blue));
            g_BattleAtbGauges[g_ActiveDrawSlot].b0 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r1 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g1 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b1 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r2 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g2 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b2 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r3 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g3 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b3 = blue;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.r = red;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.g = green;
            asm("" : : "i"(1));
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.b = blue;
        } else if ((g_FrameRngCounter & 3) == 2) {
            register int topRed asm("$7") = 0x9F;
            register int topGreen asm("$8") = 0xFF;
            register int topBlue asm("$6") = 0xF9;
            int bottomGreen = 0x46;
            int bottomBlue = 0x82;
            g_BattleAtbGauges[g_ActiveDrawSlot].r0 = topRed;
            g_BattleAtbGauges[g_ActiveDrawSlot].g0 = topGreen;
            asm volatile("" : : "r"(bottomBlue));
            g_BattleAtbGauges[g_ActiveDrawSlot].b0 = topBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r1 = 0;
            g_BattleAtbGauges[g_ActiveDrawSlot].g1 = bottomGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b1 = bottomBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r2 = topRed;
            g_BattleAtbGauges[g_ActiveDrawSlot].g2 = topGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b2 = topBlue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r3 = 0;
            g_BattleAtbGauges[g_ActiveDrawSlot].g3 = bottomGreen;
            g_BattleAtbGauges[g_ActiveDrawSlot].b3 = bottomBlue;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.r = 0;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.g = bottomGreen;
            asm("" : : "i"(2));
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.b = bottomBlue;
        } else if ((g_FrameRngCounter & 3) == 3) {
            int red = 0x50;
            int green = 0xA3;
            int blue = 0xBE;
            g_BattleAtbGauges[g_ActiveDrawSlot].r0 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g0 = green;
            asm volatile("" : : "r"(red), "r"(blue));
            g_BattleAtbGauges[g_ActiveDrawSlot].b0 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r1 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g1 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b1 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r2 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g2 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b2 = blue;
            g_BattleAtbGauges[g_ActiveDrawSlot].r3 = red;
            g_BattleAtbGauges[g_ActiveDrawSlot].g3 = green;
            g_BattleAtbGauges[g_ActiveDrawSlot].b3 = blue;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.r = red;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.g = green;
            g_BattleAtbMarkers[g_ActiveDrawSlot].sprite.color.bytes.b = blue;
        }
    }
    if (g_ActiveActor->stateFlags & 0x2000) {
        if ((g_FrameRngCounter & 3) == 0) {
            register int topRed asm("$9") = 0xFF;
            register int topGreen asm("$8") = 0x3D;
            register int topBlue asm("$7") = 0x81;
            register int bottomRed asm("$6") = 0x83;
            int bottomGreen = 0x13;
            int bottomBlue = 1;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r0 = topRed;
            asm volatile("" : : "r"(bottomBlue));
            g_BattlePeGauges[g_ActiveDrawSlot][1].g0 = topGreen;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b0 = topBlue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r1 = bottomRed;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g1 = bottomGreen;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b1 = bottomBlue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r2 = topRed;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g2 = topGreen;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b2 = topBlue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r3 = bottomRed;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g3 = bottomGreen;
            asm("" : : "i"(0));
            g_BattlePeGauges[g_ActiveDrawSlot][1].b3 = bottomBlue;
        } else if ((g_FrameRngCounter & 3) == 1) {
            register int red asm("$6") = 0xC1;
            int green = 0x28;
            int blue = 0x41;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r0 = red;
            asm volatile("" : : "r"(blue));
            g_BattlePeGauges[g_ActiveDrawSlot][1].g0 = green;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b0 = blue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r1 = red;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g1 = green;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b1 = blue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r2 = red;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g2 = green;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b2 = blue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r3 = red;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g3 = green;
            asm("" : : "i"(1));
            g_BattlePeGauges[g_ActiveDrawSlot][1].b3 = blue;
        } else if ((g_FrameRngCounter & 3) == 2) {
            register int topRed asm("$9") = 0x83;
            register int topGreen asm("$8") = 0x13;
            register int topBlue asm("$7") = 1;
            register int bottomRed asm("$6") = 0xFF;
            int bottomGreen = 0x3D;
            int bottomBlue = 0x81;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r0 = topRed;
            asm volatile("" : : "r"(bottomBlue));
            g_BattlePeGauges[g_ActiveDrawSlot][1].g0 = topGreen;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b0 = topBlue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r1 = bottomRed;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g1 = bottomGreen;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b1 = bottomBlue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r2 = topRed;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g2 = topGreen;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b2 = topBlue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r3 = bottomRed;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g3 = bottomGreen;
            asm("" : : "i"(2));
            g_BattlePeGauges[g_ActiveDrawSlot][1].b3 = bottomBlue;
        } else if ((g_FrameRngCounter & 3) == 3) {
            register int red asm("$6") = 0xC1;
            int green = 0x28;
            int blue = 0x41;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r0 = red;
            asm volatile("" : : "r"(blue));
            g_BattlePeGauges[g_ActiveDrawSlot][1].g0 = green;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b0 = blue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r1 = red;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g1 = green;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b1 = blue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r2 = red;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g2 = green;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b2 = blue;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r3 = red;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g3 = green;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b3 = blue;
        }
    }
    player = D_8009CD10;
    mode = g_ActiveActor->actionMode12;
    if (player->actionMode != mode) {
        Entity_SetActionMode(player, mode);
    }
    step = g_BattleStepIndex;
    switch (step) {
    case 0:
        /* Wait for enemy actions to finish before stopping effects. */
        entity = (BattleEntity *)g_FieldActorListHead;
        D_8009CDA8 = D_8009CDA8 | 1;
        if (entity != 0) {
            do {
                if (entity != D_8009CD10) {
                    enemy = entity->core;
                    if ((enemy != 0) && (enemy->field04.bytes.kind != 1)) {
                        if ((entity->actionMode - 2) >= 2U) {
                            if (entity->animLastFrame == entity->animPrev.parts.integer) {
                                Entity_SetActionMode(entity, (u16)(s8)enemy->lowAndEntityId.bytes.low);
                            } else if (entity->animStep != 0x10000) {
                                entity->animStep = 0x10000;
                            }
                        }
                        if (!(Battle_ProcessActionSlot(entity) & 0xFF)) {
                            ready = 0;
                        }
                    }
                }
                entity = entity->next;
            } while (entity != 0);
        }
        if (ready != 0) {
            Akao_Cmd_21(0, 0xFF);
            Pm_StopAllBoth();
            g_BattleStepIndex += 1;
            return;
        }
        return;
    case 1:
        /* Finish the resource read, restore the player and reset HUD colors. */
        if (CdRom_StepReadState(0) != step) {
            Battle_ClearMotionTable();
            nextMode = 0x18;
            if (!(g_GameStateFlags & 0x1800)) {
                player = D_8009CD10;
                nextMode = 0x15;
            } else {
                player = D_8009CD10;
                g_GameStateFlags = g_GameStateFlags & ~0x1800;
            }
            Entity_SetActionMode(player, nextMode);
            Battle_SetupPlayerPalette();
            BATTLE_SET_GAUGE_GRADIENT(g_BattleAtbGauges[0], 0, 0x46, 0x82, 0x9F, 0xFF, 0xF9);
            BATTLE_SET_SPRITE_COLOR(g_BattleAtbMarkers[0].sprite, 0x9F, 0xFF, 0xF9);
            BATTLE_SET_GAUGE_GRADIENT(g_BattleAtbGauges[1], 0, 0x46, 0x82, 0x9F, 0xFF, 0xF9);
            BATTLE_SET_SPRITE_COLOR(g_BattleAtbMarkers[1].sprite, 0x9F, 0xFF, 0xF9);
            BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
            BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
            BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][1], 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
            BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][1], 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
            g_BattleModeState = 0xC;
            Battle_ResetEnemyStats(0);
        }
        break;
    }
}

/* Finish entity death animations and restore the player for the next phase. */
void Battle_StepEntityDeath(void)
{
    BattleEntity *entity;

    switch (g_BattleStepIndex) {
    case 0:
        g_BattleEnemyEscapeFlag = 0;
        for (entity = (BattleEntity *)g_FieldActorListHead; entity != 0; entity = entity->next) {
            if (entity == D_8009CD10) continue;
            if (entity->core == 0) {
                if (entity->parent == 0) continue;
            } else {
                u32 flags;
                Entity_SetActionMode(entity,
                    (u16)(s8)((EnemyCombatant *)entity->core)->lowAndEntityId.bytes.low);
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
        g_BattleStepIndex++;
        break;
    case 1: {
        int all_done = 1;
        entity = (BattleEntity *)g_FieldActorListHead;
        if (entity != 0) {
            BattleEntity *player = D_8009CD10;
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
            g_BattleStepIndex++;
        }
        break;
    }
    case 2:
        if (CdRom_StepReadState(0) != 1) {
            Battle_ClearMotionTable();
            Entity_SetActionMode(D_8009CD10, 0x15);
            Battle_SetupPlayerPalette();
            g_BattleModeState = 11;
            Battle_ResetEnemyStats(0);
        }
        break;
    }
}

/* Start the player and battle-HUD death sequence after death cleanup. */

/* Start the player death sequence: reset the HUD gauges, stop the player and
 * spawn the death effect unless an escape is in progress. */
void Battle_StartDeathAnim(void) {
    BattleEntity *player;

    Battle_ResetEnemyStats(0);
    BATTLE_SET_GAUGE_GRADIENT(g_BattleAtbGauges[0], 0, 0x46, 0x82, 0x9F, 0xFF, 0xF9);
    BATTLE_SET_SPRITE_COLOR(g_BattleAtbMarkers[0].sprite, 0x9F, 0xFF, 0xF9);
    BATTLE_SET_GAUGE_GRADIENT(g_BattleAtbGauges[1], 0, 0x46, 0x82, 0x9F, 0xFF, 0xF9);
    BATTLE_SET_SPRITE_COLOR(g_BattleAtbMarkers[1].sprite, 0x9F, 0xFF, 0xF9);
    BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
    BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
    BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][1], 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
    BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][1], 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
    g_BattleModeState = 2;
    D_8009CDA8 |= 1;
    player = D_8009CD10;
    player->motionX = 0;
    player->motionY = 0;
    player->motionZ = 0;
    player->entityFlags &= ~0x100;
    g_GameState.flags |= 0x8000;
    if (!(g_GameStateFlags & 0x1800)) {
        Entity_SetActionMode(player, 0x14);
        Asset_Find08Alt(0x45B, 0, D_8009CD10->posX.parts.integer, D_8009CD10->posY.parts.integer,
                        D_8009CD10->posZ.parts.integer);
    }
}


void Battle_SaveAyaState(void) {
    AyaBattleState aya = g_AyaBattleStateInitial;
    BattleStateTail tail = g_SavedBattleStateTailInitial;
    BattleAttributes attributes;

    attributes.parameterWord.raw = 0;
    attributes.effectFlags = 0;
    g_AyaBattleState = aya;
    g_SavedBattleStateTail = tail;
    g_BattleEquipStateBlock = attributes;
}

void Battle_InitEquipLists(AyaBattleState **out) {
    *out = &g_AyaBattleState;
    g_AyaBattleState.action = &g_SavedBattleStateTail;
    g_AyaBattleState.attributes = &g_BattleEquipStateBlock;
    Inv_RecalcSlotStats();
    Inv_BuildWeaponList(0, &g_AyaBattleState.action->weapon);
    Inv_BuildArmorList(g_AyaBattleState.attributes);
}
