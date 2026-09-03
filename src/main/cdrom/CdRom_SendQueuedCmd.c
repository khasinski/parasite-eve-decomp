/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_cd.h"

extern CdQueuedCmdSlot D_800A3520;
extern CdQueuedCmdSlot D_800A3530;

void CdRom_SendReadyCommand(int result);
void Util_Copy8(u8 *destination, u8 *source);

int CdRom_SendQueuedCmd(u8 *destination) {
    CdQueuedCmdSlot *slot;
    u8 *copyDestination;
    int selector;

    slot = (CdQueuedCmdSlot *)destination;
    CdRom_SendReadyCommand(0);
    if (D_800A3530.state == 1) {
        selector = 4;
    } else {
        selector = D_800A3520.state == 1;
    }

    if (selector == 4) {
        copyDestination = (u8 *)slot;
        slot = &D_800A3530;
    } else if (selector == 1) {
        copyDestination = (u8 *)slot;
        slot = &D_800A3520;
    } else {
        return 0;
    }

    slot->state = 0;
    Util_Copy8(copyDestination, slot->payload);
    return slot->result;
}
