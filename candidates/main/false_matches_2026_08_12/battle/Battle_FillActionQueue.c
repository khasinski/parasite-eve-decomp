#include "pe1/battle_runtime.h"

void Battle_FillActionQueue(void) {
    BattleAction *action;
    BattleInitSlot *entry;
    int actionCount;
    int targetIndex;
    int turnIndex;

    action = D_8009D278->action;
    if ((action->turnWord & 0xc0) == 0xc0) {
        if (g_BattleTargetList[0].actor != 0) {
            targetIndex = 0;
            do {
                entry = &D_800BE830[D_8009CE3C];
                entry->actor = g_BattleTargetList[targetIndex].actor;
                entry->field04 = 2;
                entry->field06 = (s8)D_8009D2D8;
                targetIndex++;
                D_8009CE3C++;
            } while (g_BattleTargetList[targetIndex].actor != 0);
        }
        D_8009D1DC = 0;
        return;
    }

    if ((action->turnWord & 0xc0) == 0x40) {
        actionCount = ((action->turnWord & 0xf) * 3) >> 1;
        for (turnIndex = 0; turnIndex < actionCount; turnIndex++) {
            targetIndex = rand() % D_8009D2B0;
            entry = &D_800BE830[D_8009CE3C];
            entry->field06 = (s8)D_8009D2D8;
            entry->field04 = 2;
            entry->actor = g_BattleTargetList[targetIndex].actor;
            D_8009CE3C++;
        }
        D_8009D1DC = 0;
        return;
    }

    entry = &D_800BE830[D_8009CE3C];
    entry->actor = g_BattleTargetList[0].actor;
    entry->field04 = 1;
    entry->field06 = (s8)D_8009D2D8;
    D_8009CE3C++;
    D_8009D1DC--;
}
