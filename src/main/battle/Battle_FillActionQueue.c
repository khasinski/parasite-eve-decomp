/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section --expand-div */
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
u8 D_8009D2D8, D_8009CE3C, D_8009D1DC;

static inline void StoreSlot(BattleInitSlot *slot, BattleEntity *actor, s16 kind, s16 actionIndex) {
    slot->actor = actor;
    slot->field04 = kind;
    slot->field06 = actionIndex;
}

static inline void SetSlot(int index, BattleEntity *actor, s16 kind, s16 actionIndex) {
    StoreSlot(&D_800BE830[index], actor, kind, actionIndex);
}

void Battle_FillActionQueue(BattleTarget *target) {
    unsigned int mode = D_8009D278->action->turnWord & 0xC0;
    if (mode == 0xC0) {
        u8 i;
        for (i = 0; g_BattleTargetList[i].actor; i++) {
            StoreSlot(&D_800BE830[D_8009CE3C], g_BattleTargetList[i].actor, 2, (s8)D_8009D2D8);
            D_8009CE3C++;
        }
        D_8009D1DC = 0;
    } else if (mode == 0x40) {
        u8 i;
        for (i = 0; i < (int)(D_8009D278->action->turnWord & 15) * 3 / 2; i++) {
            int index = D_8009CE3C;
            int randomValue = rand();
            BattleEntity *actor = g_BattleTargetList[randomValue % D_8009D2B0].actor;
            SetSlot(index, actor, 2, (s8)D_8009D2D8);
            D_8009CE3C++;
        }
        D_8009D1DC = 0;
    } else {
        SetSlot(D_8009CE3C, target->actor, 1, (s8)D_8009D2D8);
        D_8009CE3C++;
        D_8009D1DC--;
    }
}
