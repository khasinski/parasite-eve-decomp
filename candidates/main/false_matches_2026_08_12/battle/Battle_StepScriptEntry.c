#include "pe1/battle_runtime.h"

void Battle_StepScriptEntry(void) {
    int scriptValue;

    if (D_8009D1D4 < D_8009CE3C) {
        D_8009D254->motionX = 0;
        D_8009D254->motionY = 0;
        D_8009D254->motionZ = 0;
        if (D_8009D254->actionMode < 4) {
            return;
        }

        scriptValue = D_800BE830[D_8009D1D4].field04;
        if (scriptValue < 3) {
            Battle_UpdatePlayerTurn();
        } else if (scriptValue < 0x183) {
            Battle_ApplyPlayerHit();
        } else if (scriptValue < 0x197) {
            Battle_AdvancePhase();
        } else if (scriptValue < 0x199) {
            Entity_SetActionMode(D_8009D254, 0xd);
            D_8009D2E8 |= 1;
            D_8009D1D4++;
            D_8009D254->entityFlags |= 0x100;
        } else {
            if ((signed char)Battle_RollEscapeChance() == 1) {
                Entity_SetActionMode(D_8009D254, D_8009D278->actionMode12);
                D_8009D28C = 4;
            }
            D_8009D1D4++;
        }
    } else {
        D_8009D1D4 = 0;
        D_8009CE3C = 0;
    }
}
