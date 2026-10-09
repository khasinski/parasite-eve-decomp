/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "common.h"
#include "pe1/battle.h"
#include "pe1/battle_modifiers.h"
#include "pe1/battle_saved_state.h"

/* COMMON supplies small-data metadata to stock MASPSX. The existing retail
 * symbol provides storage; this does not allocate a new global. */
BattleAttributes g_BattleEquipStateBlock;

void Battle_SaveAyaState(void) {
    AyaBattleState aya = D_80010928;
    BattleStateTail tail = D_80010998;
    BattleAttributes attributes;

    attributes.parameterWord.raw = 0;
    attributes.effectFlags = 0;
    g_AyaBattleState = aya;
    g_SavedBattleStateTail = tail;
    g_BattleEquipStateBlock = attributes;
}

void Battle_InitEquipLists(AyaBattleState **out) {
    *out = &g_AyaBattleState;
    g_AyaBattleState.action = &g_SavedBattleStateTail;
    g_AyaBattleState.attributes = &g_BattleEquipStateBlock;
    Inv_RecalcSlotStats();
    Inv_BuildWeaponList(0, &g_AyaBattleState.action->weapon);
    Inv_BuildArmorList(g_AyaBattleState.attributes);
}
