#include "pe1/battle_runtime.h"

void Battle_Init(void) {
    BattleAction *action;
    unsigned int index;

    Pm_StopAll();
    D_8009D200 = -1;
    D_8009D2FC = -1;
    D_8009D258 = -1;
    D_8009D208 = -1;
    for (index = 0; index < 45; index++) {
        D_800BE830[index].value = 0;
        D_800BE830[index].field06 = 0;
        D_800BE830[index].field04 = 0;
    }

    action = D_8009D278->action;
    D_8009CE44 = 0;
    D_8009CE40 = 0;
    D_8009D294 = 0;
    D_8009D2D8 = (action->turnWord >> 4) & 3;
    D_8009D1DC = action->turnWord & 0xf;
    D_8009CE38[0] = action->animMode[0];
    D_8009CE38[1] = action->animMode[1];
    D_8009CE38[2] = action->animMode[2];
    D_8009CE38[3] = action->animMode[3];

    Tbl_ResetAll();
    D_8009D1CE = 0;
    D_8009D1AC &= ~0x300;
    Battle_InitFadeVars();
    D_8009CE60 = 0;
}
