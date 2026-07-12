#include "../room_lib/room_lib.h"

int func_800DFB78(void);
int func_801913BC(RoomEnt *o);

int func_80190A2C(RoomEnt *o) {
    int status = func_800DFB78();

    if (status != 1) {
        if (status >= 2) {
            return 0;
        }
        if (status != 0) {
            return 0;
        }
        if (o->link->variant < 2) {
            return 0;
        }
        /* Original callback path jumps into the delay slot of the status-1 call. */
        __asm__ volatile(
            ".word 0x8E02000C\n"
            ".word 0x00000000\n"
            ".word 0x0040F809\n"
            ".word 0x02002021\n"
            ".word 0x080642A6\n"
            ".word 0x00001021");
    }
    func_801913BC(o);

    return 0;
}
