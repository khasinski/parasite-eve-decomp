/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/battle_cmd.h"

BattleCmdEntry *g_BattleCmdStackTop;
extern BattleCmdEntry g_BattleCmdStack[];

void BattleCmd_UndoPending(void);

void BattleCmd_UndoAllPending(void) {
    while (g_BattleCmdStack < g_BattleCmdStackTop) {
        BattleCmd_UndoPending();
    }
}
