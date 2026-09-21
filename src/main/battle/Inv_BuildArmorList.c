/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G0 --use-comm-section */
#include "pe1/inventory_slots.h"
#include "pe1/battle.h"
#include "pe1/battle_modifiers.h"

static inline int ClampStat(ItemDataRecord *item, int which) {
    int value = item->baseStats[which] + item->bonusStats[which];
    int result = 999;
    if (value < 1000) result = value;
    return result;
}

/* Restoring integer square root; negative inputs yield zero. */
static inline int Root(int value) {
    int root = 0, shift;
    for (shift = 30; shift >= 0; shift -= 2) {
        int trial = ((root << 2) + 1) << shift;
        root <<= 1;
        if (value >= trial) {
            value -= trial;
            root |= 1;
        }
    }
    return root;
}

void Inv_BuildArmorList(BattleAttributes *out) {
    ItemDataRecord *item;
    int value, i;
    out->effectFlags = 0;
    Inv_SelectActiveList(0);
    item = Inv_LookupActiveListData(D_800C0E20.tracked[2]);
    if (item) {
        value = ClampStat(item, 0);
        out->parameterWord.fields.first = value;
        value = ClampStat(item, 1);
        out->parameterWord.fields.second = value;
        value = item->baseStats[2] + item->bonusStats[2];
        if (value >= 1000) value = 999;
        if (value < 85) value = Root(value * 3000) / 10;
        else value = (value * 249 / 208 + 402) / 10;
        out->parameterWord.fields.third = value;
        for (i = 0; i < item->tailCount; i++) {
            int effect = item->tailData[i] & 31;
            switch (effect) {
            case 1: out->effectFlags |= 1; break;
            case 2: out->effectFlags |= 2; break;
            case 3: out->effectFlags |= 4; break;
            case 4: out->effectFlags |= 8; break;
            case 5: out->effectFlags |= 16; break;
            case 6: out->effectFlags |= 0x4000; break;
            case 7: out->effectFlags |= 0x8000; break;
            case 8: case 9: case 10:
                out->effectFlags = (out->effectFlags & ~0x1E0u) |
                    (((1 << (effect - 8)) & 15) << 5);
                break;
            case 11: out->effectFlags |= 0x800; break;
            case 12: out->effectFlags |= 0x2000; break;
            case 13: out->effectFlags |= 0x10000; break;
            case 14: out->effectFlags |= 0x200; break;
            case 15: out->effectFlags |= 0x20000; break;
            case 16: out->effectFlags |= 0x400; break;
            case 17: out->effectFlags |= 0x1000; break;
            }
        }
    } else {
        out->parameterWord.fields.first = 0;
        out->parameterWord.fields.second = 0;
        out->parameterWord.fields.third = 0;
    }
    BattleCmd_LoadWeaponModifiers();
}
