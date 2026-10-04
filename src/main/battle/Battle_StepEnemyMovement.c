/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G0 --expand-div */
#include "pe1/battle_runtime.h"
#include "pe1/math64.h"
#include "pe1/random.h"

/* Legacy name: this computes enemy damage, applies attack/status resistances,
 * subtracts HP, and starts the hit reaction. Math_Sqrt64 is the existing name
 * of the software double multiply helper. Matching debt: 2 pins and 1 empty
 * barrier retain the core alias and signed action-mode argument sequence. */
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
    activeState = D_8009D278->stateFlags;
    flagCore = enemy;
    if (activeState & 0x80000)
    {
        register s32 stat = (u16)D_8009D278->stat1E;
        double base, scale;
        stat -= 25;
        stat += D_8009D278->field04.fieldId04;
        base = Math_Int32ToDouble(stat * 6);
        scale = Math_Int32ToDouble(D_8009D2B0 - 1);
        scale = Math_Sqrt64(scale, 0.1);
        scale = __adddf3(scale, 1.0);
        attackPower = Math_DoubleToInt32(__divdf3(Math_Sqrt64(base, scale), 7.0));
    }
    else if (activeState & 0x100000)
    {
        basePower = (((u16)D_8009D278->stat1E / 5U) & 0xFFFF) + (s16) D_8009D278->action->field00;
        attackPower = ((s32) (basePower * (((D_8009D278->field04.fieldId04 * 7) + 0x78) / 160) * (s16)(D_8009D278->exp_or_acc >> 16)) / (s16) (s16)(D_8009D278->maxAtk >> 16)) + basePower;
    }
    else
    {
        unsigned char hitCountPercent[11] = { 0, 100, 60, 41, 0, 25, 0, 18, 0, 0, 13 };
        weaponAction = D_8009D278->action;
        attackPower = ((((s32)(((u16)D_8009D278->stat1E / 5U) & 0xFFFF)) + (s16) weaponAction->field00) * hitCountPercent[weaponAction->turnWord & 0xF]) / 100;
    }
    if (!(D_8009D278->stateFlags & 0x180000) && (defenseAction = D_8009D278->action, (defenseAction->actionCode.actionId != 8)))
    {
        damage = attackPower - ((s32) enemy->field8C / (s32) (defenseAction->turnWord & 0xF));
    }
    else
    {
        damage = attackPower - enemy->field8C;
    }
    if (damage > 0)
    {
        if (!(D_8009D278->stateFlags & 0x180000))
        {
            activeAction = D_8009D278->action;
            turnFlags = &activeAction->turnWord;
            hitReaction = flagCore->coreFlags & 0x38000;
            resistanceMode = 0;
            if (hitReaction == 0x8000) damage = damage * 3 / 10;
            else if (hitReaction == 0x18000) damage = damage * 3 / 2;
            else if (hitReaction == 0x20000) damage /= 10;
            attackClass = ((u32) flagCore->coreFlags >> 0x12) & 3;
            if (attackClass != 2)
            {
                if (attackClass == 3)
                {
                    (*statusFlags) |= 0x01000000;
                    goto scaleResistance;
                }
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
                        firstResistance = ((u32) (*statusFlags) >> 0xE) & 3;
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
                        poisonResistance = ((u32) (*statusFlags) >> 2) & 3;
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
                        stopResistance = ((u32) (*statusFlags) >> 4) & 3;
                        if (stopResistance != 0)
                        {
                            if (stopResistance != 2)
                            {
                            }
                            else
                            {
                                goto applyStop;
                            }
                        }
                        else
                        {
                            if (!(rand() & 1))
                            {
                            applyStop:
                                flagCore->coreFlags |= 0x1800;
                                entity->entityFlags |= 0x1000;
                                if ((s8) enemy->field04.bytes.field05 != 0)
                                {
                                    {
                                        register BattleEntity *callEntity asm("$4") = entity;
                                        register u32 mode = enemy->field06.bytes.low;
                                        asm("" : "=r"(mode) : "0"(mode), "r"(callEntity));
                                        Entity_SetActionMode(callEntity, (s8)mode & 0xFFFF);
                                    }
                                }
                                goto scaleResistance;
                            }
                        }
                    }
                }
            }
        scaleResistance:
            switch (resistanceMode & 0xFF)
            {
            case 1:
                damage /= 5;
                break;
            case 2:
                damage = (s32) (damage * 3) / 2;
                break;
            }
        }
        if (damage <= 0)
        {
            goto minimumDamage;
        }
    }
    else
    {
    minimumDamage:
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
    if (!(D_8009D278->stateFlags & 0x80000))
    {
        Battle_UpdateEntityFacing(entity);
    }
    flagCore->coreFlags = (flagCore->coreFlags & ~0x6000) | 0x4000;
}
