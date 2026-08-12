#include "pe1/battle_runtime.h"

void Battle_AdvanceTurnSlot(void) {
    BattleAction *action;
    int entryType;
    int index;
    u8 count;
    u8 savedCount;

    count = D_8009CE3C;
    D_8009CE44 = 0;
    entryType = D_800BE830[count - 1].field04;

    if (entryType == 1) {
        action = D_8009D278->action;
        count--;
        D_8009CE3C = count;
        if (D_8009D1DC == (action->turnWord & 0xf)) {
            D_8009D2D8++;
            while (count != 0 &&
                   D_800BE830[count].field06 == D_800BE830[count - 1].field06) {
                count--;
                D_8009CE3C = count;
            }
            if (D_8009CE60 != 0) {
                D_8009CE60 = 0;
                D_8009CE3C = count;
            }
        } else {
            while (count != 0 &&
                   D_800BE830[count].field06 == D_800BE830[count - 1].field06) {
                count--;
                D_8009CE3C = count;
            }
        }
    } else if (entryType == 2) {
        action = D_8009D278->action;
        if ((action->turnWord & 0xc0) == 0xc0) {
            D_8009D2D8++;
            D_8009CE3C = count - (u8)D_8009D2B0;
        } else if ((action->turnWord & 0xc0) == 0x40) {
            D_8009D2D8++;
            D_8009CE3C = count - (((action->turnWord & 0xf) * 3) >> 1);
        }
    } else if (entryType < 0x197) {
        D_8009D2D8++;
        if (D_8009CE60 == 0) {
            BattleCmd_UndoPending();
        }
        savedCount = D_8009CE3C;
        count = savedCount - 1;
        D_8009CE3C = count;
        while (count != 0 &&
               D_800BE830[count].field06 == D_800BE830[count - 1].field06) {
            count--;
            D_8009CE3C = count;
        }
        if (D_8009CE60 != 0) {
            D_8009CE60 = 0;
            D_8009CE3C = savedCount;
        }
    }

    if (entryType < 0x197) {
        D_8009D1DC = D_8009D278->action->turnWord & 0xf;
    }

    for (index = 0; index < 45; index++) {
        if (D_800BE830[index].field06 == (s8)D_8009D2D8) {
            D_800BE830[index].actor = 0;
            D_800BE830[index].field06 = 0;
            D_800BE830[index].field04 = 0;
        }
    }
}
