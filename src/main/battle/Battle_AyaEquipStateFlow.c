/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/battle.h"
#include "pe1/battle_modifiers.h"
#include "pe1/battle_saved_state.h"

extern AyaBattleState D_80010928;
extern BattleStateTail D_80010998;
extern s32 D_8009D1B4[];
#define D_8009D1B4 (D_8009D1B4[0])

void Battle_SaveAyaState(void) {
    struct {
        AyaBattleState aya;
        BattleStateTail tail;
        s32 zero0;
        s32 zero1;
    } state;

    state.aya = D_80010928;
    state.tail = D_80010998;
    state.zero0 = 0;
    state.zero1 = 0;
    g_AyaBattleState = state.aya;
    g_SavedBattleStateTail = state.tail;
    asm volatile("" ::: "memory");
    {
        volatile s32 *zeros = &state.zero0;
        g_BattleEquipStateBlock.parameterWord.raw = zeros[0];
        D_8009D1B4 = zeros[1];
    }
}

/* Preserve the G0-era absolute accesses while this TU shares the save routine's -G8 profile. */
extern void *D_800B8A88;
extern void *D_800B8A8C;
void Inv_BuildWeaponList(int arg0, void *arg1);

void Battle_InitEquipLists(void **arg0) {
    void *weaponList;
    void *armorList;

    *arg0 = &g_AyaBattleState;
    asm volatile("lui $2, %%hi(g_SavedBattleStateTail)\n\taddiu $2, $2, %%lo(g_SavedBattleStateTail)\n\tlui $1, %%hi(D_800B8A88)\n\tsw $2, %%lo(D_800B8A88)($1)" ::: "$1", "$2", "memory");
    asm volatile("lui $2, %%hi(D_8009D1B0)\n\taddiu $2, $2, %%lo(D_8009D1B0)\n\tlui $1, %%hi(D_800B8A8C)\n\tsw $2, %%lo(D_800B8A8C)($1)" ::: "$1", "$2", "memory");
    Inv_RecalcSlotStats();
    asm volatile("lui %0, %%hi(D_800B8A88)\n\tlw %0, %%lo(D_800B8A88)(%0)" : "=r"(weaponList) :: "memory");
    Inv_BuildWeaponList(0, weaponList);
    asm volatile("lui %0, %%hi(D_800B8A8C)\n\tlw %0, %%lo(D_800B8A8C)(%0)" : "=r"(armorList) :: "memory");
    Inv_BuildArmorList(armorList);
}
