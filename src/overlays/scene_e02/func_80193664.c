#include "../room_lib/room_lib.h"

extern void D_80193724(void);

#define SCHED_BARRIER() __asm__ volatile("")

int func_80193664(RoomEnt *o, int arg1, int kind, int arg3, int arg4, int arg5) {
    int value = 10;
    register int stack0 asm("t0") = arg4;
    register int stack1 asm("v1") = arg5;

    __asm__ volatile("" : "=r"(stack0), "=r"(stack1) : "0"(stack0), "1"(stack1));

    if (kind == value) {
        goto case_10;
    }
    SCHED_BARRIER();
    if (kind >= 11U) {
        goto high_kind;
    }
    value = 4;
    if (kind == value) {
        goto case_4;
    }
    goto common;

high_kind:
    value = 25;
    if (kind == value) {
        goto case_25;
    }
    value = 28;
    if (kind == value) {
        goto case_28;
    }
    goto common;

case_25:
    value = 1;
    if (arg1 != 1) {
        return 0;
    }
    o->sub.signal = (int *)arg3;
    /* Preserve the original jump-with-store delay slot; C otherwise rewrites it through v0. */
    __asm__ volatile(
        ".word 0x08064DBB\n"
        ".word 0xACE50000");

case_4:
    o->pos[0] = arg3;
    o->pos[1] = stack0;
    o->h44 = stack1;
    goto common;

case_28:
    RW16(o, 0x48) = arg3;
    o->h46 = stack0;
    goto common;

case_10:
    value = (int)D_80193724;

common:
    SCHED_BARRIER();
    o->t16 = arg3;
    RW32(o, 0xC) = value;

    SCHED_BARRIER();
    return 0;
}
