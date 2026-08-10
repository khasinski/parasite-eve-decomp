/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G0 */

#include "pe1/memcard.h"

extern MemCardPortState *D_800A1854;
extern int D_800A1858;

int MemCard_GetActiveProgressBlocks(void) {
    int blocks;
    int result;
    MemCardPortState *active = D_800A1854;

    if (active != 0) {
        blocks = (D_800A1858 - active->transferSize + 0x400) >> 10;
        result = 8;
        if (blocks < 9) {
            result = blocks;
        }
    } else {
        result = 0;
    }

    return result;
}
