#include "room_lib.h"

extern int func_800C6CE0();

void ROOMLIB_TICK_TRIGGERED_EFFECT_NAME(RoomEnt *ent, unsigned char *signal,
                                        RoomTimer2 *timer) {
    RoomTimer2 *work;
    int **flagsBase;
    int *flags;
    RoomRenderNode *target;
    unsigned short reload;

    work = timer;
    if (timer->h2A == 0) {
        if (work->h2C == 1) {
            if (func_800C6CE0(ent) == 3) {
                if (ent->link->target->state[0] == 2) {
                    reload = work->h28;
                    flagsBase = (int **)D_8009D254;
                    work->h2C = 0;
                    timer->h2A = reload;

                    flags = *flagsBase;
                    flags[0x13] |= 0x4000;

                    target = ent->link->target;
                    target->flags |= 0x80000000;
                }
            }
        }
    } else {
        work->h2A = timer->h2A - 1;
        work->h2C = 0;
    }

    if (func_800C2B68() == 1) {
        signal[1] = 2;
    }
}
