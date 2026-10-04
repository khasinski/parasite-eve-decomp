/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/battle_runtime.h"
#include "pe1/inventory.h"
#include "pe1/random.h"
#include "pe1/gte_types.h"

/* COMMON supplies small-data metadata to stock MASPSX. Existing retail
 * data/linker symbols provide storage; these do not allocate new globals. */
u8 D_8009D274, D_8009D1D4, D_8009D294;
s8 D_8009CE54, D_8009CE55;

/* Resolve the timed hit for stealing, chained targets, cone attacks or a single
 * target. Matching debt: 8 pins and 5 empty compiler barriers. Pins retain
 * retail register allocation; empty memory barriers keep
 * the attack-class store ahead of the reaction load. The resultCore barrier
 * preserves the separate steal-result branch. No instruction ASM. */
void Battle_ResolveHitOnTimer(void)
{
    register BattleEntity *resultActor;
    register s32 hitStatus;
    BattleAction *action;
    BattleEntity *selectedActor;
    BattleEntity *singleTarget;
    BattleEntity *effectTarget;
    register BattleEntity *multiTarget asm("$16");
    register BattleEntity *coneTarget;
    register BattleInitSlot *selectedSlot;
    EnemyCombatant *multiCandidate;
    register EnemyCombatant *selectedCore;
    register EnemyCombatant *multiCore;
    register EnemyCombatant *coneCandidate;
    register EnemyCombatant *coneCore;
    register EnemyCombatant *resultCore;
    s16 actionId;
    s32 stealMode;
    s32 targetAngle;
    register s16 savedReaction;
    u16 playerAngle;
    u32 multiFlags;
    u32 coneFlags;
    u8 hitTimer;
    hitTimer = D_8009D274 + 1;
    action = D_8009D278->action;
    selectedSlot = &D_800BE830[D_8009D1D4];
    D_8009D274 = hitTimer;
    if ((hitTimer & 0xFF) == action->field08)
    {
        actionId = action->actionCode.actionId;
        if (actionId == 8)
        {
            Battle_CheckEvasion(selectedSlot->actor, (BattleEvasionOutcome *)&D_8009CE54, 0);
            if (D_8009CE54 == 1)
            {
                effectTarget = selectedSlot->actor;
                Asset_Find08Alt(0x46D, 0, (s32) effectTarget->renderObject.target_x, (s32) effectTarget->renderObject.target_y, (s32) effectTarget->renderObject.target_z);
                if (D_8009D278->action->turnWord & 0x6000)
                {
                    stealMode = ((u32) ((EnemyCombatant *)selectedSlot->actor->core)->statusFlags2 >> 0xC) & 3;
                    switch (stealMode)
                    {
                        /* Retail switch shares the steal body with case 0. */
                    case 0:
                        if (!(rand() & 1))
                        {
                        case 2:
                            if (Inv_AddItem((s32) ((EnemyCombatant *)selectedSlot->actor->core)->stealItemId) != 0)
                            {
                                EnemyCombatant *enemy = selectedSlot->actor->core;
                                D_8009D1CE = 1;
                                D_8009D1F8 = Inv_GetItemEffectData((s32) enemy->stealItemId, 2);
                            }
                            else
                            {
                                EnemyCombatant *enemy = selectedSlot->actor->core;
                                D_8009D1CE = 1;
                                {
                                    void *effect = Inv_GetItemEffectData((s32) enemy->stealItemId, 1);
                                    EnemyCombatant *enemyAfter = selectedSlot->actor->core;
                                    D_8009D1F8 = effect;
                                    enemyAfter->stealItemId = 0;
                                }
                            }
                        }
                        break;
                    }
                }
            }
            resultActor = selectedSlot->actor;
            hitStatus = D_8009CE54;
            resultCore = resultActor->core;
            if (hitStatus != 1)
            {
                asm("" : : "r"(resultCore));
                goto applySelectedMiss;
            }
            goto applySelectedHit;
        }
        if (actionId == 6)
        {
            Battle_CheckEvasion(selectedSlot->actor, (BattleEvasionOutcome *)&D_8009CE54, 0);
            selectedActor = selectedSlot->actor;
            selectedCore = selectedActor->core;
            if (D_8009CE54 == 1)
            {
                register u32 flags asm("$3");
                register u32 classMask = 0xFFF3FFFF;
                register u32 reactionMask = 0xFFFC7FFF;
                Combatant *active;
                register s32 reaction;
                flags = selectedCore->coreFlags;
                flags &= ~0x6000;
                active = D_8009D278;
                flags |= 0x2000;
                selectedCore->coreFlags = flags;
                flags = (flags & classMask) | (((active->action->attackWord >> 20) & 3) << 18);
                selectedCore->coreFlags = flags;
                asm("" : : : "memory");
                reaction = D_8009CE55;
                flags = (flags & reactionMask) | ((reaction & 7) << 15);
                selectedCore->coreFlags = flags;
            }
            else if (D_8009CE54 == 0)
            {
                selectedCore->panelC_val = -1;
                selectedCore->panelC_timer = 0x1E;
                selectedCore->panelC_x = (u16) selectedActor->renderObject.projected_target_x;
                selectedCore->panelC_y = (u16) selectedActor->renderObject.projected_target_y - 0x14;
            }
            savedReaction = (s8) (u8) D_8009CE55;
            if (D_8009CE54 == 1)
            {
                multiTarget = D_8009D20C;
                if (multiTarget != 0)
                {
                nextMultiTarget:
                    if (multiTarget != D_8009D254)
                    {
                        multiCandidate = multiTarget->core;
                        if (multiCandidate != 0)
                        {
                            multiFlags = multiTarget->entityFlags;
                            if (((multiFlags & 0x2040) != 0x40) && !(multiFlags & 0x4000) && (multiCandidate->hpAlive > 0) && (multiTarget != selectedSlot->actor))
                            {
                                Battle_CheckEvasion(multiTarget, (BattleEvasionOutcome *)&D_8009CE54, (s32) (s16) savedReaction);
                                multiCore = multiTarget->core;
                                if (D_8009CE54 == 1)
                                {
                                    register u32 flags asm("$3");
                                    register u32 classMask = 0xFFF3FFFF;
                                    register u32 reactionMask = 0xFFFC7FFF;
                                    Combatant *active;
                                    register s32 reaction;
                                    flags = multiCore->coreFlags;
                                    flags &= ~0x6000;
                                    active = D_8009D278;
                                    flags |= 0x2000;
                                    multiCore->coreFlags = flags;
                                    flags = (flags & classMask) | (((active->action->attackWord >> 20) & 3) << 18);
                                    multiCore->coreFlags = flags;
                                    asm("" : : : "memory");
                                    reaction = D_8009CE55;
                                    flags = (flags & reactionMask) | ((reaction & 7) << 15);
                                    multiCore->coreFlags = flags;
                                }
                                else if (D_8009CE54 == 0)
                                {
                                    multiCore->panelC_val = -1;
                                    multiCore->panelC_timer = 0x1E;
                                    multiCore->panelC_x = (u16) multiTarget->renderObject.projected_target_x;
                                    multiCore->panelC_y = (u16) multiTarget->renderObject.projected_target_y - 0x14;
                                }
                            }
                        }
                    }
                    multiTarget = multiTarget->next;
                    if (multiTarget != 0)
                    {
                        goto nextMultiTarget;
                    }
                }
            }
        }
        else if (((action->turnWord & 0xC0) == 0x80) && !(D_8009D278->stateFlags & 0x100000))
        {
            register s32 centeredAngle;
            register s32 lowerBound;
            register s32 upperBound asm("$2");
            playerAngle = (u16) D_8009D254->facingAngle;
            upperBound = playerAngle - 0x800;
            centeredAngle = upperBound;
            upperBound = (u32)upperBound << 16;
            lowerBound = upperBound >> 16;
            if (lowerBound < -0x600)
            {
                upperBound = playerAngle + 0x600;
                lowerBound = playerAngle - 0x600;
            }
            else
            {
                upperBound = lowerBound < 0x600;
                if (upperBound)
                {
                    lowerBound = playerAngle - 0xA00;
                    upperBound = playerAngle - 0x600;
                }
                else
                {
                    lowerBound = playerAngle - 0x1600;
                    upperBound = playerAngle - 0xA00;
                }
            }
            coneTarget = D_8009D20C;
            if (coneTarget != 0)
            {
                register s32 upper asm("$18");
                register s32 lower asm("$17");
                register s32 facing;
                register int upperRange;
                register s32 shifted;
                shifted = (u32)upperBound << 16;
                upper = shifted >> 16;
                shifted = (u32)lowerBound << 16;
                lower = shifted >> 16;
                shifted = (u32)centeredAngle << 16;
                facing = shifted >> 16;
                upperRange = facing < 0x600;
            testConeTarget:
                {
                    BattleEntity *player = D_8009D254;
                    if (coneTarget != player)
                    {
                        coneCandidate = coneTarget->core;
                        if (coneCandidate != 0)
                        {
                            coneFlags = coneTarget->entityFlags;
                            if (((coneFlags & 0x2040) != 0x40) && !(coneFlags & 0x4000) && (coneCandidate->hpAlive > 0))
                            {
                                targetAngle = Gte_Atan2(coneTarget->renderObject.target_x - player->posX.parts.integer, coneTarget->renderObject.target_z - player->posZ.parts.integer);
                                if (facing >= -0x600 && upperRange)
                                {
                                    if ((s16)targetAngle < lower || upper < (s16)targetAngle) goto nextConeTarget;
                                }
                                else
                                {
                                    if ((s16)targetAngle < upper && lower < (s16)targetAngle) goto nextConeTarget;
                                }
                                {
                                    Battle_CheckEvasion(coneTarget, (BattleEvasionOutcome *)&D_8009CE54, 0);
                                    coneCore = coneTarget->core;
                                    if (D_8009CE54 == 1)
                                    {
                                        register u32 flags asm("$3");
                                        register u32 classMask = 0xFFF3FFFF;
                                        register u32 reactionMask = 0xFFFC7FFF;
                                        Combatant *active;
                                        register s32 reaction;
                                        flags = coneCore->coreFlags;
                                        flags &= ~0x6000;
                                        active = D_8009D278;
                                        flags |= 0x2000;
                                        coneCore->coreFlags = flags;
                                        flags = (flags & classMask) | (((active->action->attackWord >> 20) & 3) << 18);
                                        coneCore->coreFlags = flags;
                                        asm("" : : : "memory");
                                        reaction = D_8009CE55;
                                        flags = (flags & reactionMask) | ((reaction & 7) << 15);
                                        coneCore->coreFlags = flags;
                                    }
                                    else if (D_8009CE54 == 0)
                                    {
                                        coneCore->panelC_val = -1;
                                        coneCore->panelC_timer = 0x1E;
                                        coneCore->panelC_x = (u16) coneTarget->renderObject.projected_target_x;
                                        coneCore->panelC_y = (u16) coneTarget->renderObject.projected_target_y - 0x14;
                                    }
                                }
                            }
                        }
                    }
                }
            nextConeTarget:
                coneTarget = coneTarget->next;
                if (coneTarget != 0)
                {
                    goto testConeTarget;
                }
            }
        }
        else
        {
            singleTarget = selectedSlot->actor;
            if (singleTarget != 0)
            {
                Battle_CheckEvasion(singleTarget, (BattleEvasionOutcome *)&D_8009CE54, 0);
                resultActor = selectedSlot->actor;
                hitStatus = D_8009CE54;
                resultCore = resultActor->core;
                if (hitStatus == 1)
                {
                applySelectedHit:
                    {
                        register u32 flags asm("$3");
                        register u32 classMask = 0xFFF3FFFF;
                        register u32 reactionMask = 0xFFFC7FFF;
                        Combatant *active;
                        register s32 reaction;
                        flags = resultCore->coreFlags;
                        flags &= ~0x6000;
                        active = D_8009D278;
                        flags |= 0x2000;
                        resultCore->coreFlags = flags;
                        flags = (flags & classMask) | (((active->action->attackWord >> 20) & 3) << 18);
                        resultCore->coreFlags = flags;
                        asm("" : : : "memory");
                        reaction = D_8009CE55;
                        flags = (flags & reactionMask) | ((reaction & 7) << 15);
                        resultCore->coreFlags = flags;
                    }
                }
                else
                {
                applySelectedMiss:
                    if (hitStatus == 0)
                    {
                        resultCore->panelC_val = -1;
                        resultCore->panelC_timer = 0x1E;
                        resultCore->panelC_x = resultActor->renderObject.projected_target_x;
                        resultCore->panelC_y = resultActor->renderObject.projected_target_y - 0x14;
                    }
                }
            }
        }
        D_8009D294 = 0;
        D_8009D274 = 0;
    }
}
