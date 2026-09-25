/* MASPSX_FLAGS: --expand-div */
#include "pe1/battle_runtime.h"
#include "pe1/random.h"

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
    if (actor->entityFlags & 0x4000) {
        goto invalid;
    }
    if (D_8009D278->stateFlags & 0x100000) {
        outcome->status = 1;
        goto forced;
    }

    outcome->status = 0;
    range = 200;
    if ((D_8009D278->stateFlags & 0x30) != 0x10) {
        range = D_8009D278->action->range;
    }
    distance = Battle_CalcDistToPlayer(actor, D_8009D254) * 1000 / range;
    {
        int random = rand();
        int quotient = random / 100;
        roll = random - quotient * 100;
    }

    action = D_8009D278->action;
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
            /* The early forced result shares this output path. */
forced:
            outcome->reaction = 2;
            break;
        default:
            outcome->reaction = 4;
            break;
        }
        return;
    }

    if (distance >= 1801) goto done;
    if (distance < 1501) goto below_1501;
    hit = (s16)roll < 25;
    goto check_evasion;
below_1501:
    if (distance < 1201) goto below_1201;
    hit = (s16)roll < 50;
    goto check_evasion;
below_1201:
    if (distance < 1001) goto close_range;
    hit = (s16)roll < 80;
check_evasion:
    if (!hit) goto done;
    outcome->status = 1;
    outcome->reaction = 1;
    goto done;

close_range:
    special = (D_8009D278->action->turnWord >> 17) & 1;
    threshold = special * 20;
    if (distance >= 801) threshold += 5;
    else if (distance >= 501) threshold += 20;
    else threshold += 50;
    outcome->reaction = (s16)roll < threshold ? 3 : 2;
    outcome->status = 1;
    return;

invalid:
    outcome->status = -1;
done:
    return;
}
