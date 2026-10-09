/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "pe1/battle.h"

unsigned char g_BattleActionQueueCount;
extern int g_GameStateFlags[];
extern Combatant *g_ActiveActor[];

int Battle_IsActive(void) {
    if (g_BattleActionQueueCount != 0) {
        goto no;
    }
    if ((g_GameStateFlags[0] & 2) == 0) {
        goto yes;
    }
    if ((g_ActiveActor[0]->stateFlags & 0x10000) != 0) {
        goto no;
    }
yes:
    return 1;
no:
    return 0;
}

int Pad_IsMenuConfirmAvailable(void) {
    if (g_BattleActionQueueCount != 0) {
        goto no;
    }
    if ((g_GameStateFlags[0] & 2) == 0) {
        goto yes;
    }
    if ((g_ActiveActor[0]->stateFlags & 0x10000) != 0) {
        goto no;
    }
yes:
    return 1;
no:
    return 0;
}
