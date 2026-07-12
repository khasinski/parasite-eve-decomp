#include "../room_lib/room_lib.h"

int func_801913C4(RoomEnt *o) {
    register int state asm("$5") = 4;
    register struct RoomSub *sub asm("$6") = &o->sub;
    RoomRenderNode *target;

    o->state = state;
    o->flag3 = 0;
    if (o->bB8 != 0) {
        goto nonzero;
    }

    if (o->active == 0) {
        goto done;
    }
    target = o->link->target;
    if (target == 0) {
        goto done;
    }
    /* Original jumps to the delay-slot nop before the shared state store. */
    __asm__ volatile(
        ".word 0x8C420018\n"
        ".word 0x0806450F\n"
        ".word 0xA0450000");

nonzero:
    target = o->link->target;
    if (target != 0) {
        unsigned char *state_ptr = target->state;
        if (*state_ptr == 1) {
            o = (RoomEnt *)state_ptr;
            __asm__ volatile("");
            o->state = state;
        }
    }

done:
    if (sub->signal != 0) {
        *sub->signal = 0;
    }
    return 0;
}
