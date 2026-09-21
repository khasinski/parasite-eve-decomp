/* CC1_FLAGS: -G2 */
/* MASPSX_FLAGS: -G2 */
#include "pe1/battle_runtime.h"

void Battle_StepScriptEntry(void)
{
    BattleInitSlot *entry;
    BattleEntity *entity;
    int command;

    if (D_8009D1D4 < D_8009CE3C) {
        entry = &D_800BE830[D_8009D1D4];
        entity = D_8009D254;
        entity->motionX = 0;
        entity->motionY = 0;
        entity->motionZ = 0;
        if (entity->actionMode >= 4) {
            command = entry->field04;
            if (command < 3) {
                Battle_UpdatePlayerTurn();
            } else if (command < 0x183) {
                Battle_ApplyPlayerHit();
            } else if (command < 0x197) {
                Battle_AdvancePhase();
            } else if (command < 0x199) {
                Entity_SetActionMode(entity, 13);
                D_8009D2E8 |= 1;
                D_8009D1D4++;
                D_8009D254->entityFlags |= 0x100;
            } else {
                if ((s8)Battle_RollEscapeChance() == 1) {
                    Entity_SetActionMode(D_8009D254, D_8009D278->actionMode12);
                    D_8009D28C = 4;
                }
                D_8009D1D4++;
            }
        }
    } else {
        D_8009D1D4 = 0;
        D_8009CE3C = 0;
    }
}
