#include "pe1/battle_runtime.h"
#include "pe1/save.h"

int Battle_RollEscapeChance(void) {
    BattleEntity *entity;
    EnemyCombatant *enemy;
    Combatant *player;
    int metadataIndex;
    s16 highestRank;
    s8 chance;
    s8 rankDifference;
    s8 result;
    unsigned int escapePenalty;

    highestRank = 0;
    result = 0;
    entity = D_8009D20C;
    while (entity != 0) {
        if (entity != D_8009D254) {
            enemy = entity->core;
            if (enemy != 0 && enemy->hpAlive > 0) {
                if (highestRank < (s8)enemy->field04.bytes.rank) {
                    highestRank = (s8)enemy->field04.bytes.rank;
                }
                if (enemy->statusFlags2 & 0x40000) {
                    result = -1;
                    break;
                }
            }
        }
        entity = entity->next;
    }

    if (result == 0) {
        player = D_8009D278;
        rankDifference = (s8)(player->field04.bytes.rank - highestRank);
        if (rankDifference >= 2) {
            chance = 80;
        } else if (rankDifference == 1) {
            chance = 40;
        } else if (rankDifference == 0) {
            chance = 15;
        } else if (rankDifference == -1) {
            chance = 25;
        } else {
            chance = 0;
        }

        if (player->curHP * 10 < player->maxHP) {
            chance = (chance * 3) / 2;
        }

        escapePenalty = (player->stateFlags >> 25) & 7;
        switch (escapePenalty) {
        case 1:
            chance = (chance * 3) / 2;
            break;
        case 2:
            chance *= 2;
            break;
        case 3:
            chance *= 3;
            break;
        case 4:
            chance *= 4;
            break;
        case 5:
            chance = 100;
            break;
        }

        result = rand() % 100 < chance;
    }

    if (result <= 0) {
        D_8009D1CE = 1;
        metadataIndex = Save_GetMetadataWindowIndex();
        if (result == -1) {
            D_8009D1F8 = D_800915C0 + metadataIndex * 14;
        } else {
            D_8009D1F8 = D_8009159C + metadataIndex * 17;
        }

        player = D_8009D278;
        escapePenalty = (player->stateFlags >> 25) & 7;
        if (escapePenalty < 5) {
            player->stateFlags =
                (player->stateFlags & 0xf1ffffff) | (((escapePenalty + 1) & 7) << 25);
        }
    }

    return result;
}
