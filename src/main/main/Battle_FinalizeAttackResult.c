#include "pe1/battle_runtime.h"

void Battle_FinalizeAttackResult(void) {
    BattleAction *action;
    unsigned int attackWord;

    action = D_8009D278->action;
    if (action->actionCode.actionId == 8) {
        D_8009D294 = 1;
        Asset_Find08Alt(0x46c, 0,
                        D_8009D254->posX.parts.integer,
                        D_8009D254->posY.parts.integer,
                        D_8009D254->posZ.parts.integer);
    } else if ((action->attackWord & 0x3ff) != 0) {
        D_8009D294 = 1;
        if (!(D_8009D278->stateFlags & 0x100000)) {
            Pm_SendCmd(D_8009D200, 0, 0, 1, 0, 0);
            Akao_SetPos3D(0, 0,
                          D_8009D254->posX.parts.integer,
                          D_8009D254->posY.parts.integer,
                          D_8009D254->posZ.parts.integer);
        }
        attackWord = action->attackWord;
        action->attackWord = (attackWord & ~0x3ff) |
                             ((attackWord - 1) & 0x3ff);
    } else {
        D_8009D294 = 0;
        Asset_Find08Alt(0x46e, 0,
                        D_8009D254->posX.parts.integer,
                        D_8009D254->posY.parts.integer,
                        D_8009D254->posZ.parts.integer);
    }
}
