/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G0 --use-comm-section */
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
#include "pe1/save.h"

int Battle_RollEscapeChance(void) {
    BattleEntity *entity = D_8009D20C;
    s16 highestRank = 0;
    /* Retail narrows the probability after every multiplier. */
    s8 result = 0;
    for (; entity; entity = entity->next) {
        EnemyCombatant *enemy;
        if (entity == D_8009D254) continue;
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
        int difference = (s8)(D_8009D278->field04.bytes.rank - highestRank);
        if (difference >= 2) result = 80;
        else if (difference == 1) result = 40;
        else if (difference == 0) result = 40;
        else if (difference == -1) result = 25;
        else result = 15;
        if ((s16)D_8009D278->curHP * 10 < (s16)D_8009D278->maxHP)
            result = result * 3 / 2;
        switch (((unsigned int)D_8009D278->stateFlags >> 25) & 7) {
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
        D_8009D1CE = 1;
        if (result == -1)
            D_8009D1F8 = D_800915C0 + Save_GetMetadataWindowIndex() * 14;
        else
            D_8009D1F8 = D_8009159C + Save_GetMetadataWindowIndex() * 17;
        player = D_8009D278;
        attempts = ((unsigned int)player->stateFlags >> 25) & 7;
        if (attempts < 5)
            player->stateFlags = (player->stateFlags & 0xF1FFFFFF) |
                (((attempts + 1) & 7) << 25);
    }
    return result;
}
