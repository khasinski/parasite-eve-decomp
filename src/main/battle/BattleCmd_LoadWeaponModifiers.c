/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/battle_modifiers.h"

void BattleCmd_LoadWeaponModifiers(void)
{
    int saved = Inv_IsActiveListOverrideSelected();
    int i;
    ItemDataRecord *item;
    Inv_SelectActiveList(0);
    D_8009D018 = 0;
    for (i = 6; i >= 0; --i) D_800A1B30[i] = 0;
    /* Historical function name notwithstanding, this is the armor index. */
    item = Inv_LookupActiveListData(D_800C0E22[0]);
    if (item) {
        for (i = 0; i < item->tailCount; ++i) {
            unsigned mod = item->tailData[i] & 31;
            /* These are decoded low-five-bit values, not full ItemMod IDs. */
            switch (mod) {
            case 8: case 9: case 10: D_8009D018 = 1 << (mod - 8); break;
            case 11: D_800A1B30[0] = 3; break;
            case 12: D_800A1B30[1] = 2; break;
            case 13: D_800A1B30[5] = -2; break;
            case 15: D_800A1B30[1] = -2; break;
            }
        }
    }
    Inv_RecalcSlotStats();
    Inv_SelectActiveList(0);
    Inv_SelectActiveList(saved);
}
