/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/battle_runtime.h"

typedef struct BattleGlobalSlot {
    char storage[16];
} BattleGlobalSlot;

extern BattleGlobalSlot D_8009D278_first __asm__("D_8009D278");
extern BattleGlobalSlot D_8009D278_second __asm__("D_8009D278");
extern BattleGlobalSlot D_8009D254_large __asm__("D_8009D254");

#define ACTIVE_COMBATANT_FIRST (*(Combatant **)&D_8009D278_first)
#define ACTIVE_COMBATANT_SECOND (*(Combatant **)&D_8009D278_second)
#define PLAYER_ENTITY (*(BattleEntity **)&D_8009D254_large)

void Battle_FinalizeAttackResult(void)
{
    Combatant *combatant;
    BattleAction *action;
    u32 attack_word;

    combatant = ACTIVE_COMBATANT_FIRST;
    action = combatant->action;

    if (action->actionCode.actionId == 8) {
        BattleEntity *player;

        D_8009D294 = 1;
        player = PLAYER_ENTITY;
        Asset_Find08Alt(0x46C, 0, player->posX.parts.integer,
                        player->posY.parts.integer, player->posZ.parts.integer);
        return;
    }

    attack_word = action->attackWord;
    if ((attack_word & 0x3FF) != 0) {
        D_8009D294 = 1;
        if (!(combatant->stateFlags & 0x100000)) {
            BattleEntity *player;

            Pm_SendCmd(D_8009D200, 0, 0, 1, 0, 0);
            player = PLAYER_ENTITY;
            Akao_SetPos3D(0, 0, player->posX.parts.integer,
                          player->posY.parts.integer, player->posZ.parts.integer);
        }

        {
            Combatant *next_combatant;
            BattleAction *next_action;
            u32 next_attack_word;

            next_combatant = ACTIVE_COMBATANT_SECOND;
            next_action = next_combatant->action;
            next_attack_word = next_action->attackWord;
            next_action->attackWord = (next_attack_word & ~0x3FF) |
                                      (((next_attack_word & 0x3FF) - 1) & 0x3FF);
        }
        return;
    }

    {
        BattleEntity *player;

        D_8009D294 = 0;
        player = PLAYER_ENTITY;
        Asset_Find08Alt(0x46E, 0, player->posX.parts.integer,
                        player->posY.parts.integer, player->posZ.parts.integer);
    }
}
