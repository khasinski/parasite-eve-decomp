/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "pe1/battle_cmd.h"

extern BattleCmdEntry *D_8009D014;
extern char D_800A1B30[];

BattleCmdEntry *BattleCmd_AllocSlot(void) {
    register BattleCmdEntry *top asm("$8");
    register BattleCmdEntry *end asm("$3");
    register BattleCmdEntry *dst asm("$7");
    register BattleCmdEntry *limit asm("$10");
    BattleCmdEntry *next;

    top = D_8009D014;
    end = (BattleCmdEntry *)D_800A1B30;
    if (top < end) {
        next = top + 1;
        D_8009D014 = next;
    } else {
        top = end - 4;
        end = end - 1;
        dst = top;
        if (top < end) {
            limit = end;
            do {
                BattleCmdEntry *src = top + 1;
                *dst = *src;
                top += 1;
                dst = top;
            } while (top < limit);
        }
    }
    return top;
}
