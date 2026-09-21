/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section --expand-div */
#include "pe1/battle_runtime.h"
#include "pe1/akao.h"
#include "pe1/render_object.h"

s8 g_BattleTargetIndex;

static inline void PlayTargetSound(void) {
    void * volatile *slot = &g_AkaoBgmHandle;
    if (*slot) Akao_SendTableCommand(*slot, 0x44E, 0, 0x80, 0x7F);
}
void Battle_CycleTarget(s8 mode) {
    if (D_8009D2B0 >= 2) {
        if (mode != 1 && mode != 2) {
            if (D_8009D1F4 & 0x10) {
                Battle_MarkActiveEntities(g_BattleTargetList, g_BattleTargetIndex);
                g_BattleTargetIndex = (g_BattleTargetIndex + 1) % D_8009D2B0;
                Battle_InitFadeVars();
                PlayTargetSound();
            }
            if (D_8009D1F4 & 0x40) {
                Battle_MarkActiveEntities(g_BattleTargetList, g_BattleTargetIndex);
                g_BattleTargetIndex = (g_BattleTargetIndex + D_8009D2B0 - 1) % D_8009D2B0;
                Battle_InitFadeVars();
                PlayTargetSound();
            }
        }
    } else if (!D_8009D2B0) mode = 8;
    switch (mode) {
    case 0: case 1: case 2: case 3: {
        s8 outOfRange;
        BattleEntity *actor;
        Render_AnimationFrame();
        if ((D_8009D278->stateFlags & 0x30) == 0x10)
            outOfRange = g_BattleTargetList[g_BattleTargetIndex].dist > 200;
        else
            outOfRange = D_8009D278->action->range < g_BattleTargetList[g_BattleTargetIndex].dist;
        actor = g_BattleTargetList[g_BattleTargetIndex].actor;
        Battle_DrawStatusOverlay(&actor->renderObject, mode, outOfRange,
            ((EnemyCombatant *)actor->core)->field06.bytes.entityId);
        break;
    }
    case 4: case 5: case 6: case 7: {
        BattleEntity *actor = g_BattleTargetList[g_BattleTargetIndex].actor;
        Battle_DrawStatusOverlay(&actor->renderObject, mode, 0,
            ((EnemyCombatant *)actor->core)->field06.bytes.entityId);
        break;
    }
    case 8: Battle_DrawStatusOverlay(0, 8, 0, 0); break;
    }
    Battle_StepPlayerTurn(g_BattleTargetList, g_BattleTargetIndex, mode);
}
