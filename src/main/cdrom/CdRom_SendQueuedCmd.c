/* MASPSX_FLAGS: --stack-return-delay */

#include "pe1/psyq_cd.h"

extern CdQueuedCmdSlot D_800A3520;
extern CdQueuedCmdSlot D_800A3530;

int CdRom_SendReadyCommand(int arg);
void Util_Copy8(void *dst, void *src);

int CdRom_SendQueuedCmd(void *dst) {
    register void *dst_reg asm("$16");
    register void *dst_arg asm("$4");
    register CdQueuedCmdSlot *slot asm("$16");
    register int temp asm("$2");
    int mode;

    dst_reg = dst;
    asm volatile("" : "=r"(dst_reg) : "0"(dst_reg));
    CdRom_SendReadyCommand(0);

    if (D_800A3530.state == 1) {
        mode = 4;
    } else {
        temp = D_800A3520.state ^ 1;
        mode = (temp < 1U);
    }

    if (mode == 4) {
        dst_arg = dst_reg;
        asm volatile("" : "=r"(dst_arg) : "0"(dst_arg));
        slot = &D_800A3530;
    } else if (mode == 1) {
        dst_arg = dst_reg;
        asm volatile("" : "=r"(dst_arg) : "0"(dst_arg));
        slot = &D_800A3520;
    } else {
        return 0;
    }

    slot->state = 0;
    Util_Copy8(dst_arg, slot->payload);
    return slot->result;
}
