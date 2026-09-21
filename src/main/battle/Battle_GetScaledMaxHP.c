extern int D_800A1B30;

#include "pe1/aya.h"

int Battle_GetScaledMaxHP(int level) {
    unsigned short maxHp;

    maxHp = Aya_LookupLevelStats(level)->hp;
    return ((D_800A1B30 + 0x14) * maxHp) / 20;
}
